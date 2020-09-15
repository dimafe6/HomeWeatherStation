#include "Arduino.h"
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

float arraySum(float *array, int size)
{
    float sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += array[i];
    }

    return sum;
}

Trend getTrend(float *values, uint8_t startIndex, uint8_t size)
{
    int sumX2 = 0;
    float sumY = 0;
    float sumXY = 0;
    int trendValue = 0;

    float historyItems[size] = {0};
    for (uint8_t i = 0; i < size; i++)
    {
        historyItems[i] = values[startIndex + i];
    }

    int timeDots[size] = {0};

    if (size % 2 == 0)
    {
        timeDots[0] = ((size - 1) * -1);
        for (uint8_t n = 1; n < size; n++)
        {
            timeDots[n] = timeDots[n - 1] + 2;
        }
    }
    else
    {
        timeDots[0] = ((size - 1) / 2) * -1;
        for (uint8_t n = 1; n < size; n++)
        {
            timeDots[n] = timeDots[n - 1] + 1;
        }
    }

    for (uint8_t i = 0; i < size; i++)
    {
        sumX2 += timeDots[i] * timeDots[i];
        sumY += historyItems[i];
        sumXY += historyItems[i] * timeDots[i];
    }

    trendValue = (sumY / size + (sumXY / sumX2) * timeDots[size - 1]) - (sumY / size + (sumXY / sumX2) * timeDots[0]);

    if (trendValue > 0)
    {
        return T_RISING;
    }
    else if (trendValue == 0)
    {
        return T_STEADY;
    }
    else
    {
        return T_FALLING;
    }
}