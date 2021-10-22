#include "CO2Sensor.h"

static const char *TAG = "MHZ19";

MHZ19 MHZ19;

void initMHZ19()
{
    Serial1.begin(9600, SERIAL_8N1, RX1, TX1);
    MHZ19.begin(Serial1);
    MHZ19.setRange(5000);
    MHZ19.autoCalibration(true, 10);
}

void co2_task()
{
    internalSensorData.co2 = MHZ19.getCO2(true, true);
}