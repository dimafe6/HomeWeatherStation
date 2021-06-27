#include "Light.h"

BH1750 lightMeter;

void initBH1750()
{
    lightMeter.begin(BH1750_CONTINUOUS_LOW_RES_MODE);
}

void bh1750_task()
{
    internalSensorData.lux = lightMeter.readLightLevel();
}