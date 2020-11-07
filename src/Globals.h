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
extern float externalTemperatureLastHour[RF_SENSORS_COUNT][60];
extern float externalHumidityLast24H[RF_SENSORS_COUNT][96];
extern float externalHumidityLastHour[RF_SENSORS_COUNT][60];

extern float pressureLast24H[24];
extern uint8_t pressureLast24HmmHg[24];
extern float temperatureLast24H[96];
extern float temperatureLastHour[60];
extern float humidityLast24H[96];
extern float humidityLastHour[60];
extern float co2Last24H[96];
extern float co2LastHour[60];

extern bool shouldReboot;
extern bool shouldRedrawDisplay;

extern Median externalTemperatureFilter;
extern Median externalHumidityFilter;
extern Median internalTemperatureFilter;
extern Median internalHumidityFilter;

extern uint8_t currentOutdoorSensorId;

#endif