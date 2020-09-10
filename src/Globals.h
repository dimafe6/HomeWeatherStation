#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "Config.h"
#include "Sensors/SensorUtils.h"
#include "Filter\MedianFilter.h"

extern InternalSensorData internalSensorData;
extern InternalSensorData prevInternalSensorData;
static ExternalSensor externalSensor;
extern ExternalSensorData externalSensorData[RF_SENSORS_COUNT];
extern ExternalSensorData prevExternalSensorData[RF_SENSORS_COUNT];

extern float externalTemperatureLast24H[RF_SENSORS_COUNT][96];
extern float externalHumidityLast24H[RF_SENSORS_COUNT][96];

extern float pressureLast24H[24];
extern float temperatureLast24H[96];
extern float humidityLast24H[96];
extern unsigned int co2Last24H[96];

extern bool shouldReboot;

extern Median externalTemperatureFilter;
extern Median externalHumidityFilter;
extern Median internalTemperatureFilter;
extern Median internalHumidityFilter;

extern byte currentOutdoorSensorId;
extern byte prevOutdoorSensorId;

#endif