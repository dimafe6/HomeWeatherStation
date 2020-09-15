#include "CO2Sensor.h"

MHZ19 MHZ19;

void initMHZ19()
{
  Serial1.begin(9600, SERIAL_8N1, RX1, TX1);
  MHZ19.begin(Serial1);
  MHZ19.setRange(5000);
  MHZ19.autoCalibration(true, 10);
}

void readMHZ19Data()
{
  internalSensorData.co2 = MHZ19.getCO2(true, true);
  internalSensorData.mhz19Temperature = MHZ19.getTemperature();
}

uint32_t getCO2Color()
{
  if (internalSensorData.co2 > 0 && internalSensorData.co2 <= 600)
  {
    return 960;
  }
  else if (internalSensorData.co2 > 600 && internalSensorData.co2 <= 800)
  {
    return 4800;
  }
  else if (internalSensorData.co2 > 800 && internalSensorData.co2 <= 1000)
  {
    return 33895;
  }
  else if (internalSensorData.co2 > 1000 && internalSensorData.co2 <= 2000)
  {
    return 39622;
  }
  else if (internalSensorData.co2 > 2000 && internalSensorData.co2 <= 5000)
  {
    return 39235;
  }
  else if (internalSensorData.co2 > 5000)
  {
    return 57376;
  }
}