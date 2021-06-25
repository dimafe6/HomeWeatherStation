#ifndef RF_H
#define RF_H

#include "Arduino.h"
#include <RF24.h>
#include "Globals.h"
#include "Config.h"
#include "RTC.h"
#include "MQTT.h"

void nrf24Task(void *pvParameters);

#endif