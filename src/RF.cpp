#include "RF.h"

RF24 radio(RF_CE, RF_CSN);

const uint64_t pipes[RF_SENSORS_COUNT] = {0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL, 0xF0F0F0F096LL};
volatile bool hasExternalSensorData = false;
unsigned long lastExternalTemperatureHistoryUpdateTime = EXTERNAL_TEMPERATURE_HISTORY_INTERVAL;
unsigned long lastExternalHumidityHistoryUpdateTime = EXTERNAL_HUMIDITY_HISTORY_INTERVAL;

void initRF()
{
  radio.begin();
  Serial.println("\nRF chip status:");
  Serial.println(radio.isChipConnected());
  radio.setAutoAck(false);
  radio.setChannel(110);
  radio.disableCRC();
  radio.setPayloadSize(4);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.maskIRQ(1, 1, 0);

  radio.openWritingPipe(0xF0F0F0F0AALL);
  radio.openReadingPipe(1, pipes[0]);
  radio.openReadingPipe(2, pipes[1]);
  radio.openReadingPipe(3, pipes[2]);
  radio.openReadingPipe(4, pipes[3]);
  radio.openReadingPipe(5, pipes[4]);

  radio.startListening();

  radio.printDetails();

  pinMode(RF_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RF_IRQ), RF_ISR, FALLING);
}

void IRAM_ATTR RF_ISR()
{
  hasExternalSensorData = true;
}

void readExternalSensorData()
{
  if (hasExternalSensorData)
  {
    hasExternalSensorData = false;

    byte pipeNum = 0;
    if (radio.available(&pipeNum))
    {
      radio.read(&externalSensor, sizeof(externalSensor));

      if (externalSensor.temperature / 100 > 60 || externalSensor.temperature / 100 < -60)
      {
        return;
      }

      if (externalSensor.humidity / 100 > 100 || externalSensor.humidity / 100 < 0)
      {
        return;
      }

      prevExternalSensorData[pipeNum - 1] = externalSensorData[pipeNum - 1];

      externalSensorData[pipeNum - 1].sensorId = pipeNum;
      externalSensorData[pipeNum - 1].measurementTime = rtcEpoch32Time();

      externalSensorData[pipeNum - 1].humidity = externalHumidityFilter.filtered(float(externalSensor.humidity) / 100);
      if (externalSensorData[pipeNum - 1].humidity < externalSensorData[pipeNum - 1].humidityMin || externalSensorData[pipeNum - 1].humidityMin == NULL)
      {
        externalSensorData[pipeNum - 1].humidityMin = externalSensorData[pipeNum - 1].humidity;
      }
      if (externalSensorData[pipeNum - 1].humidity > externalSensorData[pipeNum - 1].humidityMax || externalSensorData[pipeNum - 1].humidityMax == NULL)
      {
        externalSensorData[pipeNum - 1].humidityMax = externalSensorData[pipeNum - 1].humidity;
      }

      externalSensorData[pipeNum - 1].temperature = externalTemperatureFilter.filtered(float(externalSensor.temperature) / 100);
      if (externalSensorData[pipeNum - 1].temperature < externalSensorData[pipeNum - 1].temperatureMin || externalSensorData[pipeNum - 1].temperatureMin == NULL)
      {
        externalSensorData[pipeNum - 1].temperatureMin = externalSensorData[pipeNum - 1].temperature;
      }
      if (externalSensorData[pipeNum - 1].temperature > externalSensorData[pipeNum - 1].temperatureMax || externalSensorData[pipeNum - 1].temperatureMax == NULL)
      {
        externalSensorData[pipeNum - 1].temperatureMax = externalSensorData[pipeNum - 1].temperature;
      }

      externalSensorData[pipeNum - 1].dewPoint = dewPointC(externalSensorData[pipeNum - 1].temperature, externalSensorData[pipeNum - 1].humidity);
      externalSensorData[pipeNum - 1].humIndex = humindex(externalSensorData[pipeNum - 1].temperature, externalSensorData[pipeNum - 1].dewPoint);

      // Update external temperature history
      if (millis() - lastExternalTemperatureHistoryUpdateTime > EXTERNAL_TEMPERATURE_HISTORY_INTERVAL)
      {
        lastExternalTemperatureHistoryUpdateTime = millis();

        updateExternalTemperatureHistory();
      }

      // Update external humidity history
      if (millis() - lastExternalHumidityHistoryUpdateTime > EXTERNAL_HUMIDITY_HISTORY_INTERVAL)
      {
        lastExternalHumidityHistoryUpdateTime = millis();

        updateExternalHumidityHistory();
      }

      Serial.print("Outdoor sensor ");
      Serial.print(pipeNum);
      Serial.println(":");
      Serial.print("Temp: ");
      Serial.print(externalSensorData[pipeNum - 1].temperature, 2);
      Serial.println();

      Serial.print("Humidity: ");
      Serial.print(externalSensorData[pipeNum - 1].humidity, 2);
      Serial.println();

      Serial.print("Dew point: ");
      Serial.print(externalSensorData[pipeNum - 1].dewPoint, 2);
      Serial.println();

      Serial.print("Hum index: ");
      Serial.print(externalSensorData[pipeNum - 1].humIndex, 2);
      Serial.println();
    }
  }
}

void updateExternalTemperatureHistory()
{
  for (int n = 0; n < RF_SENSORS_COUNT; n++)
  {
    for (int i = 0; i < 95; i++)
    {
      externalTemperatureLast24H[n][i] = externalTemperatureLast24H[n][i + 1];
    }

    externalTemperatureLast24H[n][95] = externalSensorData[n].temperature;
  }
}

void updateExternalHumidityHistory()
{
  for (int n = 0; n < RF_SENSORS_COUNT; n++)
  {
    for (int i = 0; i < 95; i++)
    {
      externalHumidityLast24H[n][i] = externalHumidityLast24H[n][i + 1];
    }

    externalHumidityLast24H[n][95] = externalSensorData[n].humidity;
  }
}