#include "CO2Sensor.h"

static const char *TAG = "MHZ19";

unsigned long lastCo2HistoryOneHourUpdateTime = CO2_HISTORY_ONE_HOUR_INTERVAL;

MHZ19 MHZ19;

void co2_task(void *pvParameters)
{
    Serial1.begin(9600, SERIAL_8N1, RX1, TX1);
    MHZ19.begin(Serial1);
    MHZ19.setRange(5000);
    MHZ19.autoCalibration(true, 10);

    while (1)
    {
        if (xSemaphoreTake(xGlobalVariablesMutex, portMAX_DELAY) == pdTRUE)
        {
            internalSensorData.co2 = MHZ19.getCO2(true, true);
            xSemaphoreGive(xGlobalVariablesMutex);
        } else
        {
            ESP_LOGE(TAG, "Could not obtain the semaphore xGlobalVariablesMutex from task %s", pcTaskGetTaskName(NULL));
        }

        // Update CO2 history
        if (millis() - lastCo2HistoryOneHourUpdateTime > CO2_HISTORY_ONE_HOUR_INTERVAL)
        {
            lastCo2HistoryOneHourUpdateTime = millis();

            for (int i = 0; i < 59; i++)
            {
                co2LastHour[i] = co2LastHour[i + 1];
            }
            co2LastHour[59] = (float) internalSensorData.co2;
        }

        delay(UPDATE_MHZ19_INTERVAL);
    }
}