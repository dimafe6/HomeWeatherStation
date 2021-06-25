#include "Globals.h"

SemaphoreHandle_t xGlobalVariablesMutex = NULL;
SemaphoreHandle_t xMQTTMutex;

InternalSensorData internalSensorData;
ExternalSensorData externalSensorData[RF_SENSORS_COUNT];
ExternalSensorData prevExternalSensorData[RF_SENSORS_COUNT];

float externalTemperatureLastHour[RF_SENSORS_COUNT][60] = {0};
float externalHumidityLastHour[RF_SENSORS_COUNT][60] = {0};

float pressureLast24H[24] = {0};
uint8_t pressureLast24HmmHg[24] = {0};
float temperatureLastHour[60] = {0};
float humidityLastHour[60] = {0};
float co2LastHour[60] = {0};

uint8_t currentOutdoorSensorId = 0;

char mac_address[18] = {0};
char lwt_topic[28] = {0};
char weather_condition[20] = {0};