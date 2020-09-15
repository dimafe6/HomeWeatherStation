#ifndef UTILS_H
#define UTILS_H

#define countof(a) (sizeof(a) / sizeof(a[0]))

typedef enum Trend
{
  T_RISING,
  T_STEADY,
  T_FALLING,
};

float mod(float number);
int getItegerFromFloat(float number);
int getFractionFromFloat(float number);
float getMin(float *array, int size);
float getMax(float *array, int size);
float arraySum(float *array, int size);
Trend getTrend(float *values, uint8_t startIndex, uint8_t size);

#endif