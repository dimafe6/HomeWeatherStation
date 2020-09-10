#ifndef UTILS_H
#define UTILS_H

#define countof(a) (sizeof(a) / sizeof(a[0]))

float mod(float number);
int getItegerFromFloat(float number);
int getFractionFromFloat(float number);
float getMin(float *array, int size);
float getMax(float *array, int size);

#endif