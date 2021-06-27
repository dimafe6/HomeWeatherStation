#ifndef DISPLAY_H
#define DISPLAY_H

#include <esp_task_wdt.h>
#include "Arduino.h"
#include "Config.h"
#include "Globals.h"
#include "Utils.h"
#include "Time.h"
#include "WiFi.h"
#include "./Nextion/Nextion.h"

extern NexTouch *nextionListen[];

void oHot1PushCallback(void *ptr);

void oHot2PushCallback(void *ptr);

void wifiClosePopCallback(void *ptr);

void wifiSignalPopCallback(void *ptr);

void wifiScanPopCallback(void *ptr);

void wifiScanPrevPopCallback(void *ptr);

void wifiScanNextPopCallback(void *ptr);

void wifiScanRescanPopCallback(void *ptr);

void wifiSavePopCallback(void *ptr);

void redrawDisplay();

void initDisplay();

#endif