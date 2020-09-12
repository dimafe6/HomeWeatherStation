#include "BME280.h"

BME280 bme280;

void initBME280()
{
    Wire.begin();
    Wire.setClock(400000);

    bme280.setI2CAddress(0x76);

    if (bme280.beginI2C() == false)
    {
        Serial.println("BME280 sensor connect failed");
    }

    bme280.setFilter(2);
}

void readBME280()
{
    float temp = (bme280.readTempC() * 100) / 100;
    float humidity = (bme280.readFloatHumidity() * 100) / 100;

    if (temp > 60 || temp < -60)
    {
        return;
    }

    if (humidity > 100 || humidity < 0)
    {
        return;
    }

    internalSensorData.temperature = internalTemperatureFilter.filtered(temp);
    if (internalSensorData.temperature < internalSensorData.temperatureMin || internalSensorData.temperatureMin == NULL)
    {
        internalSensorData.temperatureMin = internalSensorData.temperature;
    }
    if (internalSensorData.temperature > internalSensorData.temperatureMax || internalSensorData.temperatureMax == NULL)
    {
        internalSensorData.temperatureMax = internalSensorData.temperature;
    }

    internalSensorData.humidity = internalHumidityFilter.filtered(humidity);
    if (internalSensorData.humidity < internalSensorData.humidityMin || internalSensorData.humidityMin == NULL)
    {
        internalSensorData.humidityMin = internalSensorData.humidity;
    }
    if (internalSensorData.humidity > internalSensorData.humidityMax || internalSensorData.humidityMax == NULL)
    {
        internalSensorData.humidityMax = internalSensorData.humidity;
    }

    internalSensorData.pressure = bme280.readFloatPressure() / 100.0F; //hPa
    internalSensorData.pressureMmHg = internalSensorData.pressure / 1.33322387415F;
    internalSensorData.dewPoint = (bme280.dewPointC() * 100) / 100;
    internalSensorData.humIndex = humindex(internalSensorData.temperature, internalSensorData.dewPoint);
}
