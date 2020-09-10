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