#ifndef WIFI_H
#define WIFI_H

#include <WiFi.h>
#include "Globals.h."
#include "Config.h"
#include "RTC.h"
#include "MQTT.h"

void initWiFi();

void WiFiEvent(WiFiEvent_t event);

#endif