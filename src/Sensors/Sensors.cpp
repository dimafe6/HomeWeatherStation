#include "Sensors.h"

unsigned long lastSensorUpdateTime = UPDATE_SENSORS_INTERVAL;
unsigned long lastPressureHistoryUpdateTime = PRESSURE_HISTORY_INTERVAL;
unsigned long lastTemperatureHistoryUpdateTime = TEMPERATURE_HISTORY_INTERVAL;
unsigned long lastHumidityHistoryUpdateTime = HUMIDITY_HISTORY_INTERVAL;
unsigned long lastCo2HistoryUpdateTime = CO2_HISTORY_INTERVAL;
unsigned long lastTemperatureHistoryOneHourUpdateTime = TEMPERATURE_HISTORY_ONE_HOUR_INTERVAL;
unsigned long lastHumidityHistoryOneHourUpdateTime = HUMIDITY_HISTORY_ONE_HOUR_INTERVAL;
unsigned long lastCo2HistoryOneHourUpdateTime = CO2_HISTORY_ONE_HOUR_INTERVAL;

void readAllSensors(bool force)
{
  if ((millis() - lastSensorUpdateTime > UPDATE_SENSORS_INTERVAL) || force)
  {
    lastSensorUpdateTime = millis();

    prevInternalSensorData = internalSensorData;

    readBME280();
    readMHZ19Data();
    readLight();

    // Update pressure history
    if (millis() - lastPressureHistoryUpdateTime > PRESSURE_HISTORY_INTERVAL)
    {
      lastPressureHistoryUpdateTime = millis();

      updatePressureHistory();
    }

    // Update temperature history
    if (millis() - lastTemperatureHistoryUpdateTime > TEMPERATURE_HISTORY_INTERVAL)
    {
      lastTemperatureHistoryUpdateTime = millis();

      updateTemperatureHistory();
    }

    if (millis() - lastTemperatureHistoryOneHourUpdateTime > TEMPERATURE_HISTORY_ONE_HOUR_INTERVAL)
    {
      lastTemperatureHistoryOneHourUpdateTime = millis();

      updateTemperatureHistoryOneHour();
    }

    // Update humidity history
    if (millis() - lastHumidityHistoryUpdateTime > HUMIDITY_HISTORY_INTERVAL)
    {
      lastHumidityHistoryUpdateTime = millis();

      updateHumidityHistory();
    }

    if (millis() - lastHumidityHistoryOneHourUpdateTime > HUMIDITY_HISTORY_ONE_HOUR_INTERVAL)
    {
      lastHumidityHistoryOneHourUpdateTime = millis();

      updateHumidityHistoryOneHour();
    }

    // Update CO2 history
    if (millis() - lastCo2HistoryUpdateTime > CO2_HISTORY_INTERVAL)
    {
      lastCo2HistoryUpdateTime = millis();

      updateCO2History();
    }

    if (millis() - lastCo2HistoryOneHourUpdateTime > CO2_HISTORY_ONE_HOUR_INTERVAL)
    {
      lastCo2HistoryOneHourUpdateTime = millis();

      updateCO2HistoryOneHour();
    }

    Serial.println();
    Serial.println("Indoor sensors:");
    Serial.print("Humidity:");
    Serial.print(internalSensorData.humidity, 2);
    Serial.println();

    Serial.print("Temperature:");
    Serial.print(internalSensorData.temperature, 2);
    Serial.println();

    Serial.print("Pressure:");
    Serial.print(internalSensorData.pressure);
    Serial.println();

    Serial.print("Dew point:");
    Serial.print(internalSensorData.dewPoint, 2);
    Serial.println();

    Serial.print("Hum index:");
    Serial.print(internalSensorData.humIndex);
    Serial.println();

    Serial.print("CO2:");
    Serial.print(internalSensorData.co2);
    Serial.println();

    Serial.print("MHZ19 temp:");
    Serial.print(internalSensorData.mhz19Temperature, 2);
    Serial.println();

    Serial.print("Light:");
    Serial.print(internalSensorData.lux);
    Serial.println();
  }
}

void updatePressureHistory()
{
  for (int i = 0; i < 23; i++)
  {
    pressureLast24H[i] = pressureLast24H[i + 1];
    pressureLast24HmmHg[i] = pressureLast24HmmHg[i + 1];
  }

  pressureLast24H[23] = internalSensorData.pressure;
  pressureLast24HmmHg[23] = internalSensorData.pressureMmHg;
}

void updateTemperatureHistory()
{
  for (int i = 0; i < 95; i++)
  {
    temperatureLast24H[i] = temperatureLast24H[i + 1];
  }
  temperatureLast24H[95] = internalSensorData.temperature;
}

void updateTemperatureHistoryOneHour()
{
  for (int i = 0; i < 59; i++)
  {
    temperatureLastHour[i] = temperatureLastHour[i + 1];
  }

  temperatureLastHour[59] = internalSensorData.temperature;
}

void updateHumidityHistory()
{
  for (int i = 0; i < 95; i++)
  {
    humidityLast24H[i] = humidityLast24H[i + 1];
  }
  humidityLast24H[95] = internalSensorData.humidity;
}

void updateHumidityHistoryOneHour()
{
  for (int i = 0; i < 59; i++)
  {
    humidityLastHour[i] = humidityLastHour[i + 1];
  }
  humidityLastHour[59] = internalSensorData.humidity;
}

void updateCO2History()
{
  for (int i = 0; i < 95; i++)
  {
    co2Last24H[i] = co2Last24H[i + 1];
  }
  co2Last24H[95] = (float)internalSensorData.co2;
}

void updateCO2HistoryOneHour()
{
  for (int i = 0; i < 59; i++)
  {
    co2LastHour[i] = co2LastHour[i + 1];
  }
  co2LastHour[59] = (float)internalSensorData.co2;
}

byte getForecastImageNumber()
{
  // If no pressure for last 3h
  if (0 == pressureLast24H[21])
  {
    // Return N/A
    return getForecastImageNumberFromZambrettiChar(' ');
  }

  return getForecastImageNumberFromZambrettiChar(getZambrettiChar(internalSensorData.pressure, getTrend(pressureLast24H, 21, 3)));
}