#ifndef HOMEWEATHERSTATION_MQTT_H
#define HOMEWEATHERSTATION_MQTT_H

#include "Arduino.h"
#include "esp_wifi.h"
#include "Globals.h"
#include "EspMQTTClient.h"
#include <ArduinoJson.h>
#include "rom/rtc.h"

extern EspMQTTClient mqttClient;

void onConnectionEstablished();

bool mqttPub(const char *topic, const char *data, bool retain = false);

void mqttPubSensor(const char *topic, const char *data);

void mqtt_send_task(void *pvParameters);

void mqtt_uptime_task(void *pvParameters);

#endif
