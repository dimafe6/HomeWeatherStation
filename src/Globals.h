#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "Config.h"
#include "Sensors/SensorUtils.h"

extern SemaphoreHandle_t xGlobalVariablesMutex;
extern SemaphoreHandle_t xMQTTMutex;

extern InternalSensorData internalSensorData;
static ExternalSensor externalSensor;
extern ExternalSensorData externalSensorData[RF_SENSORS_COUNT];
extern ExternalSensorData prevExternalSensorData[RF_SENSORS_COUNT];

extern float externalTemperatureLastHour[RF_SENSORS_COUNT][60];
extern float externalHumidityLastHour[RF_SENSORS_COUNT][60];

extern float pressureLast24H[24];
extern uint8_t pressureLast24HmmHg[24];
extern float temperatureLastHour[60];
extern float humidityLastHour[60];
extern float co2LastHour[60];

extern uint8_t currentOutdoorSensorId;

extern char mac_address[18];
extern char lwt_topic[28];
extern char weather_condition[20];

extern esp_chip_info_t chipInfo;

#endif