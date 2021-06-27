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
    StaticJsonDocument<1024> doc;
    char discovery[1024];

    uint16_t unique_id_size = strlen(mac_address) + strlen(component_config_key) + 9;
    char unique_id[unique_id_size];
    snprintf(unique_id, unique_id_size, "hws-%s-%s", mac_address, component_config_key);

    uint16_t topic_size = strlen(component) + strlen(unique_id) + 25;
    char topic[topic_size];
    snprintf(topic, topic_size, "homeassistant/%s/%s/config", component, unique_id);

    uint16_t stopic_size = strlen(mac_address) + strlen(state_topic) + 8;
    char stopic[stopic_size];
    snprintf(stopic, stopic_size, "hws/%s/%s", mac_address, state_topic);

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
    char topic[100];
    snprintf(topic, 100, "hws/%s/reset_reason", mac_address);

    esp_reset_reason_t reset_reason = esp_reset_reason();

    switch (reset_reason)
    {
        case ESP_RST_UNKNOWN:
            mqttPub(topic, "Reset reason can not be determined", true);
            break;
        case ESP_RST_POWERON:
            mqttPub(topic, "Reset due to power-on event", true);
            break;
        case ESP_RST_EXT:
            mqttPub(topic, "Reset by external pin (not applicable for ESP32)", true);
            break;
        case ESP_RST_SW:
            mqttPub(topic, "Software reset via esp_restart", true);
            break;
        case ESP_RST_PANIC:
            mqttPub(topic, "Software reset due to exception/panic", true);
            break;
        case ESP_RST_INT_WDT:
            mqttPub(topic, "Reset (software or hardware) due to interrupt watchdog", true);
            break;
        case ESP_RST_TASK_WDT:
            mqttPub(topic, "Reset due to task watchdog", true);
            break;
        case ESP_RST_WDT:
            mqttPub(topic, "Reset due to other watchdogs", true);
            break;
        case ESP_RST_DEEPSLEEP:
            mqttPub(topic, "Reset after exiting deep sleep mode", true);
            break;
        case ESP_RST_BROWNOUT:
            mqttPub(topic, "Brownout reset (software or hardware)", true);
            break;
        case ESP_RST_SDIO:
            mqttPub(topic, "Reset over SDIO", true);
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
            snprintf(
                    buf,
                    100,
                    R"({"temp":%2.2f,"hum":%2.2f,"dp":%2.2f,"hi":%2.2f,"press":%i,"co2":%i})",
                    internalSensorData.temperature,
                    internalSensorData.humidity,
                    internalSensorData.dewPoint,
                    internalSensorData.humIndex,
                    internalSensorData.pressureMmHg,
                    internalSensorData.co2
            );

            mqttPubSensor("indoor", buf);
            mqttPub(lwt_topic, "online");

            for (int n = 1; n <= RF_SENSORS_COUNT; n++)
            {
                if (externalSensorData[n - 1].initialized)
                {
                    buf[0] = '\0';
                    char topic_name[12] = {0};

                    snprintf(topic_name, 12, "outdoor/%i", n);
                    snprintf(
                            buf,
                            200,
                            R"({"temp":%2.2f,"hum":%2.2f,"dp":%2.2f,"hi":%2.2f,"bat":%u})",
                            externalSensorData[n - 1].temperature,
                            externalSensorData[n - 1].humidity,
                            externalSensorData[n - 1].dewPoint,
                            externalSensorData[n - 1].humIndex,
                            externalSensorData[n - 1].battery
                    );
                    mqttPubSensor(topic_name, buf);
                }
            }
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
            char uptime[25];
            sprintf(uptime, "%i\0", esp_timer_get_time() / 1000000);
            mqttPubSensor("uptime", uptime, true);

            char sysInfo[400];

            wifi_ap_record_t wifidata;
            esp_wifi_sta_get_ap_info(&wifidata);

            tcpip_adapter_ip_info_t ipinfo;
            tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipinfo);

            char ip[18];
            char netmask[18];
            char gw[18];

            snprintf(ip, 18, IPSTR, IP2STR(&ipinfo.ip));
            snprintf(netmask, 18, IPSTR, IP2STR(&ipinfo.netmask));
            snprintf(gw, 18, IPSTR, IP2STR(&ipinfo.gw));

            snprintf(
                    sysInfo,
                    400,
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

void mqttPubSensor(const char *topic, const char *data, bool persistent)
{
    uint16_t topic_name_size = strlen(topic) + strlen(mac_address) + 8;
    char topic_name[topic_name_size];
    snprintf(topic_name, topic_name_size, "hws/%s/%s", mac_address, topic);

    mqttPub(topic_name, data, persistent);
}

void discoveryAll()
{
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
            lwt_topic,
            0
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
            "signal_strength",
            "HWS Sys info RSSI",
            "sys_info",
            "db",
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
            "bytes",
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
            "bytes",
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
        snprintf(topic, 10, "outdoor/%u", i);

        char config_key[16];
        snprintf(config_key, 16, "outdoor-%u-temp", i);
        char s_name[50];
        snprintf(s_name, 50, "HWS Outdoor %u temperature", i);
        mqttDiscovery("sensor", config_key, "temperature", s_name, topic, "°C", "{{value_json.temp}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        snprintf(config_key, 16, "outdoor-%u-hum", i);
        snprintf(s_name, 50, "HWS Outdoor %u humidity", i);
        mqttDiscovery("sensor", config_key, "humidity", s_name, topic, "%", "{{value_json.hum}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        snprintf(config_key, 16, "outdoor-%u-dp", i);
        snprintf(s_name, 50, "HWS Outdoor %u dew point", i);
        mqttDiscovery("sensor", config_key, "temperature", s_name, topic, "°C", "{{value_json.dp}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        snprintf(config_key, 16, "outdoor-%u-hi", i);
        snprintf(s_name, 50, "HWS Outdoor %u heat index", i);
        mqttDiscovery("sensor", config_key, "temperature", s_name, topic, "°C", "{{value_json.hi}}", lwt_topic);

        config_key[0] = '\0';
        s_name[0] = '\0';
        snprintf(config_key, 16, "outdoor-%u-bat", i);
        snprintf(s_name, 50, "HWS Outdoor %u battery", i);
        mqttDiscovery("sensor", config_key, "battery", s_name, topic, "%", "{{value_json.bat}}", lwt_topic);
    }
}

void onConnectionEstablished()
{
    ESP_LOGI(TAG, "MQTT connected!");

    mqttClient.subscribe("hws/weather", [](const String &topic, const String &payload) {
        strncpy(weather_condition, payload.c_str(), 20);
    });

    mqttClient.subscribe("homeassistant/status", [](const String &topic, const String &payload) {
        ESP_LOGI(TAG, "Home Assistant now is %s", payload.c_str());

        if (payload.equals("online"))
        {
            discoveryAll();
        }
    });

    discoveryAll();

    printResetReason();
}

