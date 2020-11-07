#include "Globals.h"

InternalSensorData internalSensorData;
InternalSensorData prevInternalSensorData;
ExternalSensorData externalSensorData[RF_SENSORS_COUNT];
ExternalSensorData prevExternalSensorData[RF_SENSORS_COUNT];

float externalTemperatureLast24H[RF_SENSORS_COUNT][96] = {0};
float externalTemperatureLastHour[RF_SENSORS_COUNT][60] = {0};
float externalHumidityLast24H[RF_SENSORS_COUNT][96] = {0};
float externalHumidityLastHour[RF_SENSORS_COUNT][60] = {0};

float pressureLast24H[24] = {0};
uint8_t pressureLast24HmmHg[24] = {0};
float temperatureLast24H[96] = {0};
float temperatureLastHour[60] = {0};
float humidityLast24H[96] = {0};
float humidityLastHour[60] = {0};
float co2Last24H[96] = {0};
float co2LastHour[60] = {0};

bool shouldReboot = false;
bool shouldRedrawDisplay = false;

Median externalTemperatureFilter;
Median externalHumidityFilter;
Median internalTemperatureFilter;
Median internalHumidityFilter;

uint8_t currentOutdoorSensorId = 0;