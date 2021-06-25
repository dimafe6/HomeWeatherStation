#ifndef UTILS_H
#define UTILS_H

typedef enum
{
  T_RISING,
  T_STEADY,
  T_FALLING,
} Trend;

float mod(float number);
int getIntegerFromFloat(float number);
int getFractionFromFloat(float number);
Trend getTrend(float *values, uint8_t startIndex, uint8_t size);

#endif