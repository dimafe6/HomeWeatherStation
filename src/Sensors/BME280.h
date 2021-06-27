#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include "Arduino.h"
#include <SparkFunBME280.h>
#include "../Globals.h"
#include "../Config.h"
#include "SensorUtils.h"

void initBME280();
void bme280_task();

#endif