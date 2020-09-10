#ifndef SENSOR_BME280_H
#define SENSOR_BME280_H

#include "Arduino.h"
#include <SparkFunBME280.h>
#include "../Globals.h"
#include "../Config.h"
#include "SensorUtils.h"

void initBME280();
void readBME280();

#endif