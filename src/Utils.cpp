#include "Arduino.h"
#include "Utils.h"

float mod(float number)
{
    return number >= 0 ? number : number * -1;
}

int getIntegerFromFloat(float number)
{
    int result = number * 10;

    return mod(result / 10);
}

int getFractionFromFloat(float number)
{
    return mod(int(number * 10) % 10);
}