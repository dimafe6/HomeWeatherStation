#ifndef RF_H
#define RF_H

#include "Arduino.h"
#include <RF24.h>
#include "Globals.h"
#include "Config.h"
#include "RTC.h"

void IRAM_ATTR RF_ISR();
void initRF();
void readExternalSensorData();
void updateExternalHumidityHistory();
void updateExternalTemperatureHistory();

#endif