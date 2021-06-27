#include "BME280.h"

unsigned long lastBME280UpdateTime = UPDATE_BME280_INTERVAL;
unsigned long lastPressureHistoryUpdateTime = PRESSURE_HISTORY_INTERVAL;
unsigned long lastTemperatureHistoryOneHourUpdateTime = TEMPERATURE_HISTORY_ONE_HOUR_INTERVAL;
unsigned long lastHumidityHistoryOneHourUpdateTime = HUMIDITY_HISTORY_ONE_HOUR_INTERVAL;

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

    ESP_LOGI(
            TAG,
            "Indoor sensor:\nTemp: %02.2f\nHum: %02.2f\nDew: %02.2f\nHI: %02.2f\n",
            internalSensorData.temperature,
            internalSensorData.humidity,
            internalSensorData.dewPoint,
            internalSensorData.humIndex);

    // Update pressure history
    if (millis() - lastPressureHistoryUpdateTime > PRESSURE_HISTORY_INTERVAL)
    {
        lastPressureHistoryUpdateTime = millis();

        for (int i = 0; i < 23; i++)
        {
            pressureLast24H[i] = pressureLast24H[i + 1];
            pressureLast24HmmHg[i] = pressureLast24HmmHg[i + 1];
        }

        pressureLast24H[23] = internalSensorData.pressure;
        pressureLast24HmmHg[23] = internalSensorData.pressureMmHg;
    }

    // Update temperature history
    if (millis() - lastTemperatureHistoryOneHourUpdateTime > TEMPERATURE_HISTORY_ONE_HOUR_INTERVAL)
    {
        lastTemperatureHistoryOneHourUpdateTime = millis();

        for (int i = 0; i < 59; i++)
        {
            temperatureLastHour[i] = temperatureLastHour[i + 1];
        }

        temperatureLastHour[59] = internalSensorData.temperature;
    }

    // Update humidity history
    if (millis() - lastHumidityHistoryOneHourUpdateTime > HUMIDITY_HISTORY_ONE_HOUR_INTERVAL)
    {
        lastHumidityHistoryOneHourUpdateTime = millis();

        for (int i = 0; i < 59; i++)
        {
            humidityLastHour[i] = humidityLastHour[i + 1];
        }
        humidityLastHour[59] = internalSensorData.humidity;
    }
}