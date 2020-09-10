#include "Display.h"

char displayBuffer[100] = {0};
unsigned long lastDisplayUpdateTime = 0;

NexText iTempSign = NexText(0, 20, "iTempSign");
NexText iTemp = NexText(0, 2, "iTemp");
NexText iTempTrend = NexText(0, 31, "iTempTrend");
NexText iTempFract = NexText(0, 3, "iTempFract");
NexText iTempMin = NexText(0, 10, "iTempMin");
NexText iTempMax = NexText(0, 12, "iTempMax");
NexText iDewPoint = NexText(0, 22, "iDewPoint");
NexText iHumIndex = NexText(0, 25, "iHumIndex");
NexText iHum = NexText(0, 21, "iHum");
NexText iHumTrend = NexText(0, 33, "iHumTrend");
NexText iHumMin = NexText(0, 28, "iHumMin");
NexText iHumMax = NexText(0, 26, "iHumMax");
NexWaveform iChart = NexWaveform(0, 8, "iChart");

NexText oTempSign = NexText(0, 40, "oTempSign");
NexText oTemp = NexText(0, 34, "oTemp");
NexText oTempTrend = NexText(0, 51, "oTempTrend");
NexText oTempFract = NexText(0, 35, "oTempFract");
NexText oTempMin = NexText(0, 36, "oTempMin");
NexText oTempMax = NexText(0, 38, "oTempMax");
NexText oDewPoint = NexText(0, 42, "oDewPoint");
NexText oHumIndex = NexText(0, 45, "oHumIndex");
NexText oHum = NexText(0, 41, "oHum");
NexText oHumTrend = NexText(0, 53, "oHumTrend");
NexText oHumMin = NexText(0, 48, "oHumMin");
NexText oHumMax = NexText(0, 46, "oHumMax");
NexWaveform oChart = NexWaveform(0, 9, "oChart");

NexHotspot oHot = NexHotspot(0, 55, "oHot");
NexText oSensIdx = NexText(0, 18, "oSensIdx");
NexPicture forecastImg = NexPicture(0, 19, "forecastImg");
NexButton menuBtn = NexButton(0, 54, "menuBtn");
NexText co2 = NexText(0, 14, "co2");
NexText pressure = NexText(0, 15, "pressure");
NexWaveform midChart = NexWaveform(0, 7, "midChart");

NexTouch *nextionListen[] = {&oHot, NULL};

void initDisplay()
{
  nexInit();
  oHot.attachPush(oHotPopCallback, &oHot);
}

void oHotPopCallback(void *ptr)
{
  prevOutdoorSensorId = currentOutdoorSensorId;
  currentOutdoorSensorId++;
  if (currentOutdoorSensorId > RF_SENSORS_COUNT)
  {
    currentOutdoorSensorId = 1;
  }

  memset(displayBuffer, 0, sizeof(displayBuffer));
  itoa(currentOutdoorSensorId, displayBuffer, 10);

  lastDisplayUpdateTime = UPDATE_DISPLAY_INTERVAL;
}

void redrawDisplay(bool force)
{
  if ((millis() - lastDisplayUpdateTime > UPDATE_DISPLAY_INTERVAL) || force)
  {
    lastDisplayUpdateTime = millis();

    force = force || prevOutdoorSensorId != currentOutdoorSensorId;
    int sensorIndex = currentOutdoorSensorId > 0 ? currentOutdoorSensorId - 1 : currentOutdoorSensorId;
    char buff[10] = {0};
    char output[200];

    /* Indoor */

    iTempSign.setText(internalSensorData.temperature < 0 ? "-" : " ");

    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), getItegerFromFloat(internalSensorData.temperature));
    iTemp.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR(".%d"), getFractionFromFloat(internalSensorData.temperature));
    iTempFract.setText(displayBuffer);

    switch (getTemperatureTrend(temperatureLast24H[95] - temperatureLast24H[94]))
    {
    case T_RISING:
      iTempTrend.setText("8");
      break;
    case T_FALLING:
      iTempTrend.setText("9");
      break;
    default:
      iTempTrend.setText(":");
      break;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.temperatureMin), displayBuffer, 10);
    iTempMin.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.temperatureMax), displayBuffer, 10);
    iTempMax.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.dewPoint), displayBuffer, 10);
    iDewPoint.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.humIndex), displayBuffer, 10);
    iHumIndex.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), getItegerFromFloat(internalSensorData.humidity));
    iHum.setText(displayBuffer);

    switch (getHumidityTrend(humidityLast24H[95] - humidityLast24H[94]))
    {
    case T_RISING:
      iHumTrend.setText("8");
      break;
    case T_FALLING:
      iHumTrend.setText("9");
      break;
    default:
      iHumTrend.setText(":");
      break;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.humidityMin), displayBuffer, 10);
    iHumMin.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.humidityMax), displayBuffer, 10);
    iHumMax.setText(displayBuffer);

    /* Outdoor */

    oTempSign.setText(externalSensorData[sensorIndex].temperature < 0 ? "-" : " ");

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), getItegerFromFloat(externalSensorData[sensorIndex].temperature));
    oTemp.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR(".%d"), getFractionFromFloat(externalSensorData[sensorIndex].temperature));
    oTempFract.setText(displayBuffer);

    switch (getTemperatureTrend(externalTemperatureLast24H[sensorIndex][95] - externalTemperatureLast24H[sensorIndex][94]))
    {
    case T_RISING:
      oTempTrend.setText("8");
      break;
    case T_FALLING:
      oTempTrend.setText("9");
      break;
    default:
      oTempTrend.setText(":");
      break;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[sensorIndex].temperatureMin), displayBuffer, 10);
    oTempMin.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[sensorIndex].temperatureMax), displayBuffer, 10);
    oTempMax.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[sensorIndex].dewPoint), displayBuffer, 10);
    oDewPoint.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[sensorIndex].humIndex), displayBuffer, 10);
    oHumIndex.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), getItegerFromFloat(externalSensorData[sensorIndex].humidity));
    oHum.setText(displayBuffer);

    switch (getHumidityTrend(externalHumidityLast24H[sensorIndex][95] - externalHumidityLast24H[sensorIndex][94]))
    {
    case T_RISING:
      oHumTrend.setText("8");
      break;
    case T_FALLING:
      oHumTrend.setText("9");
      break;
    default:
      oHumTrend.setText(":");
      break;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[sensorIndex].humidityMin), displayBuffer, 10);
    oHumMin.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[sensorIndex].humidityMax), displayBuffer, 10);
    oHumMax.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(currentOutdoorSensorId, displayBuffer, 10);
    oSensIdx.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.co2), displayBuffer, 10);
    co2.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.pressure), displayBuffer, 10);
    pressure.setText(displayBuffer);

    //TODO: forecast
  }
}
