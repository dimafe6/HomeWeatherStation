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

NexHotspot oHot = NexHotspot(0, 60, "oHot");
NexText oSensIdx = NexText(0, 15, "oSensIdx");
NexText oSignal = NexText(0, 14, "oSignal");
NexText oBatt = NexText(0, 59, "oBatt");
NexPicture forecastImg = NexPicture(0, 19, "forecastImg");
NexButton menuBtn = NexButton(0, 54, "menuBtn");
NexText co2 = NexText(0, 11, "co2");
NexText co2Trend = NexText(0, 58, "co2Trend");
NexText pressure = NexText(0, 15, "pressure");
NexText pressureTrend = NexText(0, 59, "pressureTrend");
NexWaveform midChart = NexWaveform(0, 7, "midChart");

NexTouch *nextionListen[] = {&oHot, NULL};

NexText dateText = NexText(0, 53, "date");
NexText hourText = NexText(0, 52, "hour");
NexText minuteText = NexText(0, 56, "minute");

NexTimer timeDotsTimer = NexTimer(0, 57, "timeDotsTimer");

void initDisplay()
{
  nexInit();
  oHot.attachPush(oHotPopCallback, &oHot);
  timeDotsTimer.disable();
}

void oHotPopCallback(void *ptr)
{
  currentOutdoorSensorId++;
  if (currentOutdoorSensorId >= RF_SENSORS_COUNT)
  {
    currentOutdoorSensorId = 0;
  }

  memset(displayBuffer, 0, sizeof(displayBuffer));
  itoa(currentOutdoorSensorId, displayBuffer, 10);

  shouldRedrawDisplay = true;
}

void redrawDisplay(bool force)
{
  force = force || shouldRedrawDisplay;

  if ((millis() - lastDisplayUpdateTime > UPDATE_DISPLAY_INTERVAL) || force)
  {
    lastDisplayUpdateTime = millis();

    if (shouldRedrawDisplay)
    {
      shouldRedrawDisplay = false;
    }

    char buff[10] = {0};
    char output[200];

    if (year() > 2000)
    {
      timeDotsTimer.enable();
    }

    /* Indoor */

    iTempSign.setText(internalSensorData.temperature < 0 ? "-" : " ");

    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), getItegerFromFloat(internalSensorData.temperature));
    iTemp.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR(".%d"), getFractionFromFloat(internalSensorData.temperature));
    iTempFract.setText(displayBuffer);

    switch (getTrend(temperatureLastHour, 0, 60))
    {
    case T_RISING:
      iTempTrend.setText("8");
      Serial.print("\nTemp trend: ");
      Serial.println("Rising");
      break;
    case T_FALLING:
      iTempTrend.setText("9");
      Serial.print("\nTemp trend: ");
      Serial.println("Falling");
      break;
    default:
      iTempTrend.setText(":");
      Serial.print("\nTemp trend: ");
      Serial.println("Steady");
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

    switch (getTrend(humidityLastHour, 0, 60))
    {
    case T_RISING:
      iHumTrend.setText("8");
      Serial.print("\nHumidity trend: ");
      Serial.println("Rising");
      break;
    case T_FALLING:
      iHumTrend.setText("9");
      Serial.print("\nHumidity trend: ");
      Serial.println("Falling");
      break;
    default:
      iHumTrend.setText(":");
      Serial.print("\nHumidity trend: ");
      Serial.println("Steady");
      break;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.humidityMin), displayBuffer, 10);
    iHumMin.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.humidityMax), displayBuffer, 10);
    iHumMax.setText(displayBuffer);

    /* Outdoor */

    oTempSign.setText(externalSensorData[currentOutdoorSensorId].temperature < 0 ? "-" : " ");

    switch (externalSensorData[currentOutdoorSensorId].signal)
    {
    case 1:
      oSignal.setText("5"); // Full signal
      oSignal.Set_font_color_pco(0);
      break;
    case 0:
      oSignal.setText("3"); // Low signal
      oSignal.Set_font_color_pco(0);
      break;
    default:
      oSignal.setText("0"); // No signal
      oSignal.Set_font_color_pco(57376); //Red
      break;
    }

    if (externalSensorData[currentOutdoorSensorId].battery == 255)
    {
      oBatt.setText("0"); // No battery
      oBatt.Set_font_color_pco(57376);
    }
    else if (externalSensorData[currentOutdoorSensorId].battery >= 0 && externalSensorData[currentOutdoorSensorId].battery <= 5)
    {
      oBatt.setText("1"); // Critical battery
      oBatt.Set_font_color_pco(57376); // Red
    }
    else if (externalSensorData[currentOutdoorSensorId].battery >= 6 && externalSensorData[currentOutdoorSensorId].battery <= 25)
    {
      oBatt.setText("2"); // Low battery
      oBatt.Set_font_color_pco(0);
    }
    else if (externalSensorData[currentOutdoorSensorId].battery >= 26 && externalSensorData[currentOutdoorSensorId].battery <= 50)
    {
      oBatt.setText("3"); // Half battery
      oBatt.Set_font_color_pco(0);
    }
    else if (externalSensorData[currentOutdoorSensorId].battery >= 51 && externalSensorData[currentOutdoorSensorId].battery <= 75)
    {
      oBatt.setText("4"); // Good battery
      oBatt.Set_font_color_pco(0);
    }
    else if (externalSensorData[currentOutdoorSensorId].battery >= 76)
    {
      oBatt.setText("5"); // Full battery
      oBatt.Set_font_color_pco(0);
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), getItegerFromFloat(externalSensorData[currentOutdoorSensorId].temperature));
    oTemp.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR(".%d"), getFractionFromFloat(externalSensorData[currentOutdoorSensorId].temperature));
    oTempFract.setText(displayBuffer);

    switch (getTrend(externalTemperatureLastHour[currentOutdoorSensorId], 0, 60))
    {
    case T_RISING:
      oTempTrend.setText("8");
      Serial.print("\nExternal Temp trend: ");
      Serial.println("Rising");
      break;
    case T_FALLING:
      oTempTrend.setText("9");
      Serial.print("\nExternal Temp trend: ");
      Serial.println("Falling");
      break;
    default:
      oTempTrend.setText(":");
      Serial.print("\nExternal Temp trend: ");
      Serial.println("Steady");
      break;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[currentOutdoorSensorId].temperatureMin), displayBuffer, 10);
    oTempMin.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[currentOutdoorSensorId].temperatureMax), displayBuffer, 10);
    oTempMax.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[currentOutdoorSensorId].dewPoint), displayBuffer, 10);
    oDewPoint.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[currentOutdoorSensorId].humIndex), displayBuffer, 10);
    oHumIndex.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), getItegerFromFloat(externalSensorData[currentOutdoorSensorId].humidity));
    oHum.setText(displayBuffer);

    switch (getTrend(externalHumidityLastHour[currentOutdoorSensorId], 0, 60))
    {
    case T_RISING:
      oHumTrend.setText("8");
      Serial.print("\nExternal Hum trend: ");
      Serial.println("Rising");
      break;
    case T_FALLING:
      oHumTrend.setText("9");
      Serial.print("\nExternal Hum trend: ");
      Serial.println("Falling");
      break;
    default:
      oHumTrend.setText(":");
      Serial.print("\nExternal Hum trend: ");
      Serial.println("Steady");
      break;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[currentOutdoorSensorId].humidityMin), displayBuffer, 10);
    oHumMin.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(externalSensorData[currentOutdoorSensorId].humidityMax), displayBuffer, 10);
    oHumMax.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(currentOutdoorSensorId + 1, displayBuffer, 10);
    oSensIdx.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(getItegerFromFloat(internalSensorData.co2), displayBuffer, 10);
    co2.setText(displayBuffer);
    co2.Set_font_color_pco(getCO2Color());

    switch (getTrend(co2LastHour, 0, 3))
    {
    case T_RISING:
      co2Trend.setText("8");
      Serial.print("\nCO2 trend: ");
      Serial.println("Rising");
      break;
    case T_FALLING:
      co2Trend.setText("9");
      Serial.print("\nCO2 trend: ");
      Serial.println("Falling");
      break;
    default:
      co2Trend.setText(":");
      Serial.print("\nCO2 trend: ");
      Serial.println("Steady");
      break;
    }
    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(internalSensorData.pressureMmHg, displayBuffer, 10);
    pressure.setText(displayBuffer);

    if (0 == pressureLast24H[21])
    {
      pressureTrend.setText(":");
    }
    else
    {
      switch (getTrend(pressureLast24H, 21, 3))
      {
      case T_RISING:
        pressureTrend.setText("8");
        Serial.print("\nPressure trend: ");
        Serial.println("Rising");
        break;
      case T_FALLING:
        pressureTrend.setText("9");
        Serial.print("\nPressure trend: ");
        Serial.println("Falling");
        break;
      default:
        pressureTrend.setText(":");
        Serial.print("\nPressure trend: ");
        Serial.println("Steady");
        break;
      }
    }

    /* Date and time */

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), hour());
    hourText.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer, countof(displayBuffer), PSTR("%02d"), minute());
    minuteText.setText(displayBuffer);

    memset(displayBuffer, 0, sizeof(displayBuffer));
    snprintf_P(displayBuffer,
               countof(displayBuffer),
               PSTR("%02u.%02u.%02u"),
               day(),
               month(),
               year() - 2000);
    dateText.setText(displayBuffer);

    /* Forecast */
    forecastImg.setPic(getForecastImageNumber());
  }
}
