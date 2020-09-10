#include "Sensors.h"

unsigned long lastSensorUpdateTime = UPDATE_SENSORS_INTERVAL;
unsigned long lastPressureHistoryUpdateTime = PRESSURE_HISTORY_INTERVAL;
unsigned long lastTemperatureHistoryUpdateTime = TEMPERATURE_HISTORY_INTERVAL;
unsigned long lastHumidityHistoryUpdateTime = HUMIDITY_HISTORY_INTERVAL;
unsigned long lastCo2HistoryUpdateTime = CO2_HISTORY_INTERVAL;

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

    // Update humidity history
    if (millis() - lastHumidityHistoryUpdateTime > HUMIDITY_HISTORY_INTERVAL)
    {
      lastHumidityHistoryUpdateTime = millis();

      updateHumidityHistory();
    }

    // Update CO2 history
    if (millis() - lastCo2HistoryUpdateTime > CO2_HISTORY_INTERVAL)
    {
      lastCo2HistoryUpdateTime = millis();

      updateCO2History();
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
  }
  pressureLast24H[23] = internalSensorData.pressure;

  /* TEST */
  float trend = pressureLast24H[23] - pressureLast24H[20];
  Serial.println("Pressure trend");
  Serial.println(getPressureTrend(trend));
  Serial.println("Trend value");
  Serial.println(trend);
  Serial.println("Zambretti char");
  Serial.println(getZambrettiChar(internalSensorData.pressure, getPressureTrend(trend)));
  Serial.println("Explanation");
  Serial.println(getZambrettiExplanation(getZambrettiChar(internalSensorData.pressure, getPressureTrend(trend))));
  /* TEST */
}

void updateTemperatureHistory()
{
  for (int i = 0; i < 95; i++)
  {
    temperatureLast24H[i] = temperatureLast24H[i + 1];
  }
  temperatureLast24H[95] = internalSensorData.temperature;

  int min = getMin(temperatureLast24H, 96);
  int max = getMax(temperatureLast24H, 96);

  if (min == max)
  {
    min = 0;
  }
}

void updateHumidityHistory()
{
  for (int i = 0; i < 95; i++)
  {
    humidityLast24H[i] = humidityLast24H[i + 1];
  }
  humidityLast24H[95] = internalSensorData.humidity;
}

void updateCO2History()
{
  for (int i = 0; i < 95; i++)
  {
    co2Last24H[i] = co2Last24H[i + 1];
  }
  co2Last24H[95] = internalSensorData.co2;

  /* TEST */
  float trend = co2Last24H[95] - co2Last24H[94];
  Serial.println("CO2 trend");
  Serial.println(getCO2Trend(trend));
  Serial.println("Trend value");
  Serial.println(trend);
  /* TEST */
}
