#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#include "Arduino.h"
#include "BH1750.h"
#include "../Globals.h"
#include "../Config.h"
#include "SensorUtils.h"

void initBH1750();
void bh1750_task();

#endif