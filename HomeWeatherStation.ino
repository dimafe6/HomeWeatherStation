#include "Arduino.h"
#include "./src/Globals.h"
#include "./src/Nextion/Nextion.h"
#include "./src/RF.h"
#include "./src/RTC.h"
#include "./src/Radio.h"
#include "./src/Display.h"
#include "./src/WiFi.h"
#include "./src/Sensors/CO2Sensor.h"
#include "./src/Sensors/BME280.h"
#include "./src/Sensors/Light.h"

#define _TASK_TIMECRITICAL
#define _TASK_SLEEP_ON_IDLE_RUN

#include <TaskScheduler.h>

static const char *TAG = "MAIN";

Task task_nrf24(500, TASK_FOREVER, &nrf24_task);
Task task_co2(UPDATE_MHZ19_INTERVAL, TASK_FOREVER, &co2_task);
Task task_bme280(UPDATE_BME280_INTERVAL, TASK_FOREVER, &bme280_task);
Task task_bh1750(UPDATE_BH1750_INTERVAL, TASK_FOREVER, &bh1750_task);

Scheduler runner;

void setup()
{
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
    mqttClient.setMqttReconnectionAttemptDelay(2000);
    mqttClient.enableDebuggingMessages(true);

    initDisplay();
    initRtc();
    initWiFi();
    initRF();
    initMHZ19();
    initBH1750();
    initBME280();

    runner.init();
    runner.addTask(task_nrf24);
    runner.addTask(task_co2);
    runner.addTask(task_bme280);
    runner.addTask(task_bh1750);
    task_nrf24.enable();
    task_co2.enable();
    task_bme280.enable();
    task_bh1750.enable();
}

void loop()
{
    redrawDisplay();
    nexLoop(nextionListen);
    mqttClient.loop();

    if (WiFi.status() == WL_CONNECTED)
    {
        if (timeClient.update())
        {
            syncTimeFromNTP();
        }
    }

    runner.execute();
}
