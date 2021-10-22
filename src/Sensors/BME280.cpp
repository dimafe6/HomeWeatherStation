#include "BME280.h"

static const char *TAG = "BME280";

unsigned long lastBME280UpdateTime = UPDATE_BME280_INTERVAL;

BME280 bme280;

void initBME280()
{
    bme280.setI2CAddress(0x76);

    if (bme280.beginI2C() == false)
    {
        ESP_LOGE(TAG, "BME280 sensor connect failed");
    }

    bme280.setFilter(2);
    bme280.setTempOverSample(2);
    bme280.setPressureOverSample(2);
    bme280.setHumidityOverSample(2);
    bme280.setStandbyTime(5);
    bme280.setMode(MODE_NORMAL);
}

void bme280_task()
{
    while (bme280.isMeasuring())
    {
        delay(1);
    }

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

    internalSensorData.temperature = temp;
    internalSensorData.humidity = humidity;

    if (internalSensorData.initialized)
    {
        internalSensorData.temperatureMin = min(internalSensorData.temperatureMin,
                                                internalSensorData.temperature);
        internalSensorData.temperatureMax = max(internalSensorData.temperatureMax,
                                                internalSensorData.temperature);
        internalSensorData.humidityMin = min(internalSensorData.humidityMin, internalSensorData.humidity);
        internalSensorData.humidityMax = max(internalSensorData.humidityMax, internalSensorData.humidity);
    } else
    {
        internalSensorData.temperatureMin = internalSensorData.temperature;
        internalSensorData.temperatureMax = internalSensorData.temperature;
        internalSensorData.humidityMin = internalSensorData.humidity;
        internalSensorData.humidityMax = internalSensorData.humidity;

        internalSensorData.initialized = true;
    }

    internalSensorData.pressure = bme280.readFloatPressure() / 100.0F; //hPa
    internalSensorData.pressureMmHg = internalSensorData.pressure / 1.33322387415F;
    internalSensorData.dewPoint = bme280.dewPointC() * 100 / 100;
    internalSensorData.humIndex = humindex(internalSensorData.temperature, internalSensorData.dewPoint);

}