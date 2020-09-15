#ifndef SENSOR_CO2SENSOR_H
#define SENSOR_CO2SENSOR_H

#include "Arduino.h"
#include "MHZ19.h"
#include "../Globals.h"
#include "../Config.h"

void initMHZ19();
void readMHZ19Data();
uint32_t getCO2Color();
#endif