#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "Config.h"
#include "Sensors/SensorUtils.h"
#include <Preferences.h>

extern SemaphoreHandle_t xMQTTMutex;

extern InternalSensorData internalSensorData;
static ExternalSensor externalSensor;
extern ExternalSensorData externalSensorData[RF_SENSORS_COUNT];
extern ExternalSensorData prevExternalSensorData[RF_SENSORS_COUNT];

extern uint8_t currentOutdoorSensorId;

extern char mac_address[18];
extern char lwt_topic[28];
extern char weather_condition[20];

extern esp_chip_info_t chipInfo;

extern Preferences preferences;

#endif