#ifndef I2C_SENSORS_H
#define I2C_SENSORS_H

#include "Arduino.h"
#include <SparkFunBME280.h>
#include "BH1750.h"
#include "../Globals.h"
#include "../Config.h"
#include "SensorUtils.h"

void i2cSensorsTask(void *pvParameters);

#endif