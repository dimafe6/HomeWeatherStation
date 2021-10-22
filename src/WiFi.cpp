#include "WiFi.h"

static const char *TAG = "WIFI";

void initWiFi()
{
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    WiFi.onEvent(WiFiEvent);

    ESP_LOGI(TAG, "Connecting to WIFI");
    WiFi.begin();
    WiFi.setSleep(false);

    xTaskCreatePinnedToCore(mqtt_send_task, "mqtt_send_task", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(mqtt_uptime_task, "mqtt_uptime_task", configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL,
                            APP_CPU_NUM);
}

void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
        case SYSTEM_EVENT_WIFI_READY:
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            ESP_LOGI(TAG, "Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            break;
        case SYSTEM_EVENT_STA_STOP:
            ESP_LOGI(TAG, "WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected from WiFi access point");
            timeClient.end();

            if (WiFi.getAutoReconnect())
            {
                WiFi.reconnect();
            }
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            ESP_LOGI(TAG, "Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            timeClient.begin();
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            ESP_LOGI(TAG, "Lost IP address and IP address is reset to 0");
            if (WiFi.getAutoReconnect())
            {
                WiFi.reconnect();
            }
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            ESP_LOGI(TAG, "WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            ESP_LOGI(TAG, "WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            ESP_LOGI(TAG, "WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            ESP_LOGI(TAG, "WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            ESP_LOGI(TAG, "WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            ESP_LOGI(TAG, "WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            ESP_LOGI(TAG, "Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            ESP_LOGI(TAG, "Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            ESP_LOGI(TAG, "IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            ESP_LOGI(TAG, "Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            ESP_LOGI(TAG, "Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            ESP_LOGI(TAG, "Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            ESP_LOGI(TAG, "Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            ESP_LOGI(TAG, "Obtained IP address");
            break;
    }
}
