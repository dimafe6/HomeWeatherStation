#ifndef SENSORS_H
#define SENSORS_H

#include "BME280.h"
#include "CO2Sensor.h"
#include "LightSensor.h"
#include "SensorUtils.h"
#include "../Utils.h"

void readAllSensors(bool force);
void updatePressureHistory();
void updateTemperatureHistory();
void updateHumidityHistory();
void updateCO2History();
byte getForecastImageNumber();

#endif