#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H

#include "Arduino.h"
#include <TimeLib.h>
#include "../Utils.h"

struct InternalSensorData
{
  int co2;
  float mhz19Temperature;
  float temperature;
  float temperatureMin = NULL;
  float temperatureMax = NULL;
  float humidity;
  float humidityMin = NULL;
  float humidityMax = NULL;
  float pressure; //hPa
  int pressureMmHg;
  float dewPoint;
  int16_t humIndex;
  int lux;
};

struct __attribute__((__packed__)) ExternalSensor
{
  int16_t temperature;
  int16_t humidity;
};

struct ExternalSensorData
{
  byte sensorId;
  float temperature;
  float temperatureMin = NULL;
  float temperatureMax = NULL;
  float humidity;
  float humidityMin = NULL;
  float humidityMax = NULL;
  float dewPoint;
  float humIndex;
  uint32_t measurementTime = 0;
};

double dewPointC(double celsius, double humidity);

long humindex(float temp, float dewPoint);

String getZambrettiExplanation(char c);

char getZambrettiChar(float P, Trend trend);

byte getForecastImageNumberFromZambrettiChar(char zambrettiChar);

#endif