#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include "Arduino.h"
#include "../Utils.h"

struct InternalSensorData
{
    bool initialized = false;
    int16_t co2;
    float temperature;
    float temperatureMin = 0;
    float temperatureMax = 0;
    float humidity;
    float humidityMin = 0;
    float humidityMax = 0;
    float pressure; //hPa
    int16_t pressureMmHg;
    float dewPoint;
    float humIndex;
    int lux;
};

struct __attribute__((__packed__)) ExternalSensor
{
    int16_t temperature;
    int16_t humidity;
    uint8_t battery;
};

struct ExternalSensorData
{
    bool initialized = false;
    uint8_t sensorId = 255;
    float temperature = 0;
    float temperatureMin = 0;
    float temperatureMax = 0;
    float humidity = 0;
    float humidityMin = 0;
    float humidityMax = 0;
    float dewPoint = 0;
    float humIndex = 0;
    uint32_t measurementTime = 0;
    uint16_t sleepTime = 0;
    uint8_t battery = 255;
    uint8_t signal = 255;
};

double dewPointC(double celsius, double humidity);

long humindex(float temp, float dewPoint);

#endif