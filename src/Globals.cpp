#include "Globals.h"

SemaphoreHandle_t xMQTTMutex;

InternalSensorData internalSensorData;
ExternalSensorData externalSensorData[RF_SENSORS_COUNT];
ExternalSensorData prevExternalSensorData[RF_SENSORS_COUNT];

uint8_t currentOutdoorSensorId = 0;

char mac_address[18] = {0};
char lwt_topic[28] = {0};
char weather_condition[20] = {0};

Preferences preferences;