#include "RF.h"

static const char *TAG = "RF";

RF24 radio(RF_CE, RF_CSN);

const uint64_t pipes[RF_SENSORS_COUNT] = {0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL, 0xF0F0F0F0A5LL,
                                          0xF0F0F0F096LL};
unsigned long lastExternalTemperatureHistoryOneHourUpdateTime = EXTERNAL_TEMPERATURE_HISTORY_ONE_HOUR_INTERVAL;
unsigned long lastExternalHumidityHistoryOneHourUpdateTime = EXTERNAL_HUMIDITY_HISTORY_ONE_HOUR_INTERVAL;
unsigned long lastSensorSignalCheckTime = UPDATE_SENSORS_INTERVAL;

void initRF()
{
    radio.begin();
    ESP_LOGI(TAG, "RF chip status: %i", radio.isChipConnected());
    radio.setAutoAck(false);
    radio.setChannel(80);
    radio.disableCRC();
    radio.setPayloadSize(5);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.maskIRQ(1, 1, 0);

    radio.openWritingPipe(0xF0F0F0F0AALL);
    radio.openReadingPipe(1, pipes[0]);
    radio.openReadingPipe(2, pipes[1]);
    radio.openReadingPipe(3, pipes[2]);
    radio.openReadingPipe(4, pipes[3]);
    radio.openReadingPipe(5, pipes[4]);

    radio.startListening();

    //radio.printDetails();
}

void nrf24_task()
{
    uint8_t pipeNum = 0;
    if (radio.available(&pipeNum))
    {
        radio.read(&externalSensor, sizeof(externalSensor));

        if (externalSensor.temperature >= -6000 && externalSensor.temperature <= 6000 &&
            externalSensor.humidity >= 0 && externalSensor.humidity <= 10000)
        {
            if (externalSensor.temperature != 0)
            {
                externalSensor.temperature /= 100;
            }

            if (externalSensor.humidity != 0)
            {
                externalSensor.humidity /= 100;
            }

            prevExternalSensorData[pipeNum - 1] = externalSensorData[pipeNum - 1];

            externalSensorData[pipeNum - 1].sensorId = pipeNum;
            externalSensorData[pipeNum - 1].measurementTime = now();

            if (externalSensor.battery >= 0 && externalSensor.battery <= 100)
            {
                externalSensorData[pipeNum - 1].battery = externalSensor.battery;
            }

            externalSensorData[pipeNum - 1].signal = radio.testRPD();
            externalSensorData[pipeNum - 1].humidity = externalSensor.humidity;
            externalSensorData[pipeNum - 1].temperature = externalSensor.temperature;

            if (prevExternalSensorData[pipeNum - 1].initialized)
            {
                externalSensorData[pipeNum - 1].sleepTime = externalSensorData[pipeNum - 1].measurementTime -
                                                            prevExternalSensorData[pipeNum - 1].measurementTime;
            }

            if (externalSensorData[pipeNum - 1].initialized)
            {
                externalSensorData[pipeNum - 1].humidityMin = min(
                        externalSensorData[pipeNum - 1].humidityMin,
                        externalSensorData[pipeNum - 1].humidity
                );
                externalSensorData[pipeNum - 1].humidityMax = max(
                        externalSensorData[pipeNum - 1].humidityMax,
                        externalSensorData[pipeNum - 1].humidity
                );
                externalSensorData[pipeNum - 1].temperatureMin = min(
                        externalSensorData[pipeNum - 1].temperatureMin,
                        externalSensorData[pipeNum - 1].temperature
                );
                externalSensorData[pipeNum - 1].temperatureMax = max(
                        externalSensorData[pipeNum - 1].temperatureMax,
                        externalSensorData[pipeNum - 1].temperature
                );
            } else
            {
                externalSensorData[pipeNum - 1].humidityMin = externalSensorData[pipeNum - 1].humidity;
                externalSensorData[pipeNum - 1].humidityMax = externalSensorData[pipeNum - 1].humidity;
                externalSensorData[pipeNum - 1].temperatureMin = externalSensorData[pipeNum - 1].temperature;
                externalSensorData[pipeNum - 1].temperatureMax = externalSensorData[pipeNum - 1].temperature;

                externalSensorData[pipeNum - 1].initialized = true;
            }

            externalSensorData[pipeNum - 1].dewPoint = dewPointC(
                    externalSensorData[pipeNum - 1].temperature,
                    externalSensorData[pipeNum - 1].humidity
            );
            externalSensorData[pipeNum - 1].humIndex = humindex(
                    externalSensorData[pipeNum - 1].temperature,
                    externalSensorData[pipeNum - 1].dewPoint
            );

            ESP_LOGI(
                    TAG,
                    "Sensor %i:\nTemp: %02.2f\nHum: %02.2f\nHum(min): %02.2f\nDew:  %02.2f\nHI: %02.2f\nBat: %i",
                    pipeNum,
                    externalSensorData[pipeNum - 1].temperature,
                    externalSensorData[pipeNum - 1].humidity,
                    externalSensorData[pipeNum - 1].humidityMin,
                    externalSensorData[pipeNum - 1].dewPoint,
                    externalSensorData[pipeNum - 1].humIndex,
                    externalSensorData[pipeNum - 1].battery);

            // Update external temperature history
            if (millis() - lastExternalTemperatureHistoryOneHourUpdateTime >
                EXTERNAL_TEMPERATURE_HISTORY_ONE_HOUR_INTERVAL)
            {
                lastExternalTemperatureHistoryOneHourUpdateTime = millis();

                for (int n = 0; n < RF_SENSORS_COUNT; n++)
                {
                    for (int i = 0; i < 59; i++)
                    {
                        externalTemperatureLastHour[n][i] = externalTemperatureLastHour[n][i + 1];
                    }

                    externalTemperatureLastHour[n][59] = externalSensorData[n].temperature;
                }
            }

            // Update external humidity history
            if (millis() - lastExternalHumidityHistoryOneHourUpdateTime >
                EXTERNAL_HUMIDITY_HISTORY_ONE_HOUR_INTERVAL)
            {
                lastExternalHumidityHistoryOneHourUpdateTime = millis();

                for (int n = 0; n < RF_SENSORS_COUNT; n++)
                {
                    for (int i = 0; i < 59; i++)
                    {
                        externalHumidityLastHour[n][i] = externalHumidityLastHour[n][i + 1];
                    }

                    externalHumidityLastHour[n][59] = externalSensorData[n].humidity;
                }
            }
        } else
        {
            ESP_LOGI(
                    TAG,
                    "Wrong sensor %i data:\nTemp(x100): %i\nHum(x100): %i\n",
                    pipeNum,
                    externalSensor.temperature,
                    externalSensor.humidity);
        }
    }

    if (millis() - lastSensorSignalCheckTime > UPDATE_SENSORS_INTERVAL)
    {
        lastSensorSignalCheckTime = millis();

        for (uint8_t i = 0; i < RF_SENSORS_COUNT; i++)
        {
            if (!externalSensorData[i].initialized)
            {
                continue;
            }

            if (externalSensorData[i].sleepTime > 0 && externalSensorData[i].measurementTime > 0)
            {
                uint32_t lastDataReceiveSeconds = now() - externalSensorData[i].measurementTime;
                uint32_t losedMessages = lastDataReceiveSeconds / externalSensorData[i].sleepTime;

                if (losedMessages > 0)
                {
                    ESP_LOGW(TAG, "Loosed %i message(s) from sensor id %i", losedMessages, i + 1);
                }

                if (losedMessages >= RF_MAX_LOSSS_MESSAGES_BEFORE_LOSE_SIGNAL)
                {
                    externalSensorData[i].temperature = 0;
                    externalSensorData[i].temperatureMin = 0;
                    externalSensorData[i].temperatureMax = 0;
                    externalSensorData[i].humidity = 0;
                    externalSensorData[i].humidityMin = 0;
                    externalSensorData[i].humidityMax = 0;
                    externalSensorData[i].dewPoint = 0;
                    externalSensorData[i].humIndex = 0;
                    externalSensorData[i].battery = 255;
                    externalSensorData[i].signal = 255;
                }
            }
        }
    }
}
