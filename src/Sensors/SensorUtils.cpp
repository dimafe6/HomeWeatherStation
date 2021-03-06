#include "SensorUtils.h"

double dewPointC(double celsius, double humidity)
{
    // (1) Saturation Vapor Pressure = ESGG(T)
    double RATIO = 373.15 / (273.15 + celsius);
    double RHS = -7.90298 * (RATIO - 1);
    RHS += 5.02808 * log10(RATIO);
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO))) - 1);
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1);
    RHS += log10(1013.246);
    // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;
    // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP / 0.61078); // temp var
    return (241.88 * T) / (17.558 - T);
}

long humindex(float temp, float dewPoint)
{
    return round(temp + 0.5555 * (6.11 * exp(5417.753 * (1 / 273.16 - 1 / (273.15 + dewPoint))) - 10));
}