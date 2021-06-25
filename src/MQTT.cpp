#include "MQTT.h"

static const char *TAG = "WIFI";

static uint8_t publish_retries = 0;

EspMQTTClient mqttClient(
        MQTT_SERVER,
        MQTT_PORT,
        MQTT_CLIENT_NAME
);

static void mqttDiscovery(const char *component,
                          const char *component_config_key,
                          const char *device_class,
                          const char *component_name,
                          const char *state_topic,
                          const char *unit,
                          const char *value_template,
                          const char *availability_topic = "",
                          uint16_t expire_after = 60,
                          const char *payload_off = "",
                          const char *payload_on = "")
{
    StaticJsonDocument<2048> doc;
    char discovery[2048];

    char unique_id[strlen(mac_address) + strlen(component_config_key) + 6];
    sprintf(unique_id, "hws-%s-%s", mac_address, component_config_key);

    char topic[strlen(component) + strlen(unique_id) + 22];
    sprintf(topic, "homeassistant/%s/%s/config", component, unique_id);

    char stopic[strlen(mac_address) + strlen(state_topic) + 6];
    sprintf(stopic, "hws/%s/%s", mac_address, state_topic);

    if (strlen(device_class))
    {
        doc["dev_cla"] = device_class;
    }

    if (strlen(component_name))
    {
        doc["name"] = component_name;
    }

    if (strlen(unit))
    {
        doc["unit_of_meas"] = unit;
    }

    if (strlen(value_template))
    {
        doc["val_tpl"] = value_template;
    }

    if (expire_after > 0)
    {
        doc["exp_aft"] = expire_after;
    }

    if (strlen(availability_topic))
    {
        doc["avty_t"] = availability_topic;
    }

    doc["uniq_id"] = unique_id;
    doc["stat_t"] = stopic;
    doc["qos"] = 2;

    if (strcmp(component, "binary_sensor") == 0)
    {
        doc["payload_off"] = payload_off;
        doc["payload_on"] = payload_on;
    }

    JsonObject dev = doc.createNestedObject("dev");
    dev["ids"] = mac_address;

    JsonArray dev_connections_0 = dev["connections"].createNestedArray();
    dev_connections_0.add("mac");
    dev_connections_0.add(mac_address);

    dev["mf"] = "Dmytro Feshchenko";
    dev["name"] = "Home weather station";
    dev["mdl"] = "Home weather station";

    serializeJson(doc, discovery);

    if (!mqttPub(topic, discovery, true))
    {
        ESP_LOGE(TAG, "MQTT! publish failed");
    }
}

static void printResetReason()
{
    char topic[20];
    sprintf(topic, "hws/%s/reset_reason", mac_address);

    RESET_REASON reason = rtc_get_reset_reason(0);

    switch (reason)
    {
        case 1 :
            mqttPub(topic, "Vbat power on reset");
            break;
        case 3 :
            mqttPub(topic, "Software reset digital core");
            break;
        case 4 :
            mqttPub(topic, "Legacy watch dog reset digital core");
            break;
        case 5 :
            mqttPub(topic, "Deep Sleep reset digital core");
            break;
        case 6 :
            mqttPub(topic, "Reset by SLC module, reset digital core");
            break;
        case 7 :
            mqttPub(topic, "Timer Group0 Watch dog reset digital core");
            break;
        case 8 :
            mqttPub(topic, "Timer Group1 Watch dog reset digital core");
            break;
        case 9 :
            mqttPub(topic, "RTC Watch dog Reset digital core");
            break;
        case 10 :
            mqttPub(topic, "Instruction tested to reset CPU");
            break;
        case 11 :
            mqttPub(topic, "Time Group reset CPU");
            break;
        case 12 :
            mqttPub(topic, "Software reset CPU");
            break;
        case 13 :
            mqttPub(topic, "RTC Watch dog Reset CPU");
            break;
        case 14 :
            mqttPub(topic, "Reseted by PRO CPU");
            break;
        case 15 :
            mqttPub(topic, "Reset when the vdd voltage is not stable");
            break;
        case 16 :
            mqttPub(topic, "RTC Watch dog reset digital core and rtc module");
            break;
    }
}

void mqtt_send_task(void *pvParameters)
{
    while (true)
    {
        if (mqttClient.isMqttConnected())
        {
            char buf[100] = {0};
            sprintf(
                    buf,
                    R"({"temp":%2.2f,"hum":%2.2f,"dp":%2.2f,"hi":%2.2f,"press":%i,"co2":%i})",
                    internalSensorData.temperature,
                    internalSensorData.humidity,
                    internalSensorData.dewPoint,
                    internalSensorData.humIndex,
                    internalSensorData.pressureMmHg,
                    internalSensorData.co2);

            mqttPubSensor("indoor", buf);
            mqttPub(lwt_topic, "online");
        }

        delay(MQTT_SEND_INTERVAL);
    }
}

void mqtt_uptime_task(void *pvParameters)
{
    while (true)
    {
        if (mqttClient.isMqttConnected())
        {
            char uptime[20];
            sprintf(uptime, "%i", esp_timer_get_time() / 1000000);
            mqttPubSensor("uptime", uptime);

            char sysInfo[400];

            wifi_ap_record_t wifidata;
            esp_wifi_sta_get_ap_info(&wifidata);

            tcpip_adapter_ip_info_t ipinfo;
            tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipinfo);

            char ip[18];
            char netmask[18];
            char gw[18];

            sprintf(ip, IPSTR, IP2STR(&ipinfo.ip));
            sprintf(netmask, IPSTR, IP2STR(&ipinfo.netmask));
            sprintf(gw, IPSTR, IP2STR(&ipinfo.gw));

            sprintf(
                    sysInfo,
                    R"({"ssid":"%s","rssi":%i,"ip":"%s","netmask":"%s","gw":"%s","free_heap":%i,"free_heap_min":%i,"mac_address":"%s"})",
                    wifidata.ssid,
                    wifidata.rssi,
                    ip,
                    netmask,
                    gw,
                    esp_get_free_heap_size(),
                    esp_get_minimum_free_heap_size(),
                    mac_address
            );

            mqttPubSensor("sys_info", sysInfo);
        }

        delay(5000);
    }
}

bool mqttPub(const char *topic, const char *data, bool retain)
{
    if (xSemaphoreTake(xMQTTMutex, portMAX_DELAY) == pdTRUE)
    {
        //ESP_LOGI(TAG, "Publish MQTT message: %s to topic %s", data, topic);

        bool send = mqttClient.publish(topic, data);

        xSemaphoreGive(xMQTTMutex);

        if (!send)
        {
            ESP_LOGE(TAG, "Cannot publish MQTT message to topic %s. Retrying...", topic);

            publish_retries++;

            if (publish_retries < 5)
            {
                delay(100);
                mqttPub(topic, data, retain);
            } else if (publish_retries >= 5)
            {
                publish_retries = 0;
            }
        } else
        {
            publish_retries = 0;
            delay(10);
        }

        return send;
    } else
    {
        ESP_LOGE(TAG, "Could not obtain the semaphore xMQTTMutex from task %s", pcTaskGetTaskName(NULL));
    }

    return false;
}

void mqttPubSensor(const char *topic, const char *data)
{
    char topic_name[strlen(topic) + strlen(mac_address) + 6];
    sprintf(topic_name, "hws/%s/%s", mac_address, topic);

    mqttPub(topic_name, data);
}

void onConnectionEstablished()
{
    ESP_LOGI(TAG, "MQTT connected!");

    mqttClient.subscribe("hws/weather", [](const String &topic, const String &payload) {
        strcpy(weather_condition, payload.c_str());
    });

    //MQTT discovery
    mqttDiscovery(
            "sensor",
            "indoor-temp",
            "temperature",
            "HWS Indoor temperature",
            "indoor",
            "°C",
            "{{value_json.temp}}",
            lwt_topic
    );

    mqttDiscovery(
            "sensor",
            "indoor-hum",
            "humidity",
            "HWS Indoor humidity",
            "indoor",
            "%",
            "{{value_json.hum}}",
            lwt_topic
    );

    mqttDiscovery(
            "sensor",
            "indoor-press",
            "pressure",
            "HWS Indoor pressure",
            "indoor",
            "mmHg",
            "{{value_json.press}}",
            lwt_topic
    );

    mqttDiscovery(
            "sensor",
            "indoor-dp",
            "temperature",
            "HWS Indoor dew point",
            "indoor",
            "°C",
            "{{value_json.dp}}",
            lwt_topic
    );

    mqttDiscovery(
            "sensor",
            "indoor-hi",
            "temperature",
            "HWS Indoor heat index",
            "indoor",
            "°C",
            "{{value_json.hi}}",
            lwt_topic
    );

    mqttDiscovery(
            "sensor",
            "indoor-co2",
            "carbon_dioxide",
            "HWS Indoor CO₂",
            "indoor",
            "ppm",
            "{{value_json.co2}}",
            lwt_topic
    );

    mqttDiscovery(
            "binary_sensor",
            "status",
            "",
            "HWS status",
            "lwt",
            "",
            "",
            "",
            0,
            "offline",
            "online"
    );

    mqttDiscovery(
            "sensor",
            "uptime",
            "",
            "HWS Uptime",
            "uptime",
            "",
            R"({% set uptime = (value_json | int ) %})"
            R"({% set seconds = uptime %})"
            R"({% set minutes = ((uptime % 3600) / 60) | int %})"
            R"({% set hours = ((uptime % 86400) / 3600) | int %})"
            R"({% set days = (uptime / 86400) | int %})"
            R"({%- if days > 0 -%})"
            R"({%- if days == 1 -%})"
            R"(1 day)"
            R"({%- else -%})"
            R"({{ days }} days)"
            R"({%- endif -%})"
            R"({{ ', ' }})"
            R"({%- endif -%})"
            R"({{ '%02d' % hours }}:{{ '%02d' % minutes }})",
            lwt_topic
    );

    mqttDiscovery(
            "sensor",
            "reset_reason",
            "",
            "HWS Reset cause",
            "reset_reason",
            "",
            "",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-ssid",
            "",
            "HWS Sys info SSID",
            "sys_info",
            "",
            "{{value_json.ssid}}",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-rssi",
            "",
            "HWS Sys info RSSI",
            "sys_info",
            "",
            "{{value_json.rssi}}",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-ip",
            "",
            "HWS Sys info IP",
            "sys_info",
            "",
            "{{value_json.ip}}",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-netmask",
            "",
            "HWS Sys info netmask",
            "sys_info",
            "",
            "{{value_json.netmask}}",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-gw",
            "",
            "HWS Sys info gw",
            "sys_info",
            "",
            "{{value_json.gw}}",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-free_heap",
            "",
            "HWS Sys info free heap",
            "sys_info",
            "",
            "{{value_json.free_heap}}",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-free_heap_min",
            "",
            "HWS Sys info free heap min",
            "sys_info",
            "",
            "{{value_json.free_heap_min}}",
            lwt_topic,
            0
    );

    mqttDiscovery(
            "sensor",
            "sys_info-mac_address",
            "",
            "HWS Sys info MAC",
            "sys_info",
            "",
            "{{value_json.mac_address}}",
            lwt_topic,
            0
    );

    for (uint8_t i = 1; i <= RF_SENSORS_COUNT; i++)
    {
        char topic[10];
        sprintf(topic, "outdoor/%i", i);

        char config_key[15];
        sprintf(config_key, "outdoor-%i-temp", i);
        char s_name[50];
        sprintf(s_name, "HWS Outdoor %i temperature", i);
        mqttDiscovery("sensor", config_key, "temperature", s_name, topic, "°C", "{{value_json.temp}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        sprintf(config_key, "outdoor-%i-hum", i);
        sprintf(s_name, "HWS Outdoor %i humidity", i);
        mqttDiscovery("sensor", config_key, "humidity", s_name, topic, "%", "{{value_json.hum}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        sprintf(config_key, "outdoor-%i-dp", i);
        sprintf(s_name, "HWS Outdoor %i dew point", i);
        mqttDiscovery("sensor", config_key, "temperature", s_name, topic, "°C", "{{value_json.dp}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        sprintf(config_key, "outdoor-%i-hi", i);
        sprintf(s_name, "HWS Outdoor %i heat index", i);
        mqttDiscovery("sensor", config_key, "temperature", s_name, topic, "°C", "{{value_json.hi}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        sprintf(config_key, "outdoor-%i-bat", i);
        sprintf(s_name, "HWS Outdoor %i battery", i);
        mqttDiscovery("sensor", config_key, "battery", s_name, topic, "%", "{{value_json.bat}}", lwt_topic);
    }

    printResetReason();
}

