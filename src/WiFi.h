#ifndef WIFI_H
#define WIFI_H

#include <WiFi.h>
#include "Secrets.h"
#include "Config.h"

void initWiFi();
void WiFiEvent(WiFiEvent_t event);

#endif