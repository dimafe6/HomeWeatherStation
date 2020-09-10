#include "Utils.h"

float mod(float number)
{
    return number >= 0 ? number : number * -1;
}

int getItegerFromFloat(float number)
{
    int result = number * 10;

    return mod(result / 10);
}

int getFractionFromFloat(float number)
{
    return mod(int(number * 10) % 10);
}

float getMin(float *array, int size)
{
    int minimum = array[0];
    for (int i = 0; i < size; i++)
    {
        if (array[i] < minimum)
            minimum = array[i];
    }
    return minimum;
}

float getMax(float *array, int size)
{
    int maximum = array[0];
    for (int i = 0; i < size; i++)
    {
        if (array[i] > maximum)
            maximum = array[i];
    }
    return maximum;
}