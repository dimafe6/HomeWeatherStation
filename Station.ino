#include "Arduino.h"
#include "./src/Globals.h"
#include "./src/Nextion/Nextion.h"
#include "./src/RF.h"
#include "./src/RTC.h"
#include "./src/Radio.h"
#include "./src/Display.h"
#include "./src/WiFi.h"
#include "./src/Sensors/CO2Sensor.h"
#include "./src/Sensors/I2CSensors.h"

static const char *TAG = "MAIN";

void setup()
{
    vSemaphoreCreateBinary(xGlobalVariablesMutex);
    vSemaphoreCreateBinary(xMQTTMutex);

    esp_log_level_set("*", ESP_LOG_DEBUG);
    Serial.begin(115200);
    Wire.begin();

    ESP_LOGI(TAG, "Initializing...");

    uint8_t mac_bytes[6];
    esp_read_mac(mac_bytes, ESP_MAC_WIFI_STA);
    snprintf(
            mac_address,
            18,
            "%02x-%02x-%02x-%02x-%02x-%02x",
            mac_bytes[0],
            mac_bytes[1],
            mac_bytes[2],
            mac_bytes[3],
            mac_bytes[4],
            mac_bytes[5]
    );

    snprintf(lwt_topic, 28, "hws/%s/lwt", mac_address);

    mqttClient.enableLastWillMessage(lwt_topic, "offline");
    mqttClient.setMaxPacketSize(2048);

    initDisplay();
    initRtc();
    initWiFi();

    xTaskCreatePinnedToCore(nrf24Task, "nrf24Task", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(co2_task, "co2_task", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL, APP_CPU_NUM);
    xTaskCreatePinnedToCore(i2cSensorsTask, "i2cSensorsTask", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL, APP_CPU_NUM);
}

void loop()
{
    redrawDisplay();
    nexLoop(nextionListen);
    mqttClient.loop();

    if (WiFi.status() == WL_CONNECTED)
    {
        if(timeClient.update()) {
            syncTimeFromNTP();
        }
    }
}
