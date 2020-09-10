#ifndef SENSOR_LIGHT_H
#define SENSOR_LIGHT_H

#include "Arduino.h"
#include "BH1750.h"
#include "../Globals.h"
#include "../Config.h"

void initLightMeter();
void readLight();
#endif