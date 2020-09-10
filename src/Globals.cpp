#include "Globals.h"

InternalSensorData internalSensorData;
InternalSensorData prevInternalSensorData;
ExternalSensorData externalSensorData[RF_SENSORS_COUNT];
ExternalSensorData prevExternalSensorData[RF_SENSORS_COUNT];

float externalTemperatureLast24H[RF_SENSORS_COUNT][96] = {0};
float externalHumidityLast24H[RF_SENSORS_COUNT][96] = {0};

float pressureLast24H[24] = {0};
float temperatureLast24H[96] = {0};
float humidityLast24H[96] = {0};
unsigned int co2Last24H[96] = {0};

bool shouldReboot = false;

Median externalTemperatureFilter;
Median externalHumidityFilter;
Median internalTemperatureFilter;
Median internalHumidityFilter;

byte currentOutdoorSensorId = 1;
byte prevOutdoorSensorId = 0;