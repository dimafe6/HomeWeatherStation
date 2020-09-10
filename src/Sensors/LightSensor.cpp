#include "LightSensor.h"

BH1750 lightMeter;

void initLightMeter()
{
  lightMeter.begin(BH1750_CONTINUOUS_LOW_RES_MODE);
}

void readLight()
{
  internalSensorData.lux = lightMeter.readLightLevel();
}