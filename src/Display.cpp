#include "Display.h"
#include "WiFi.h"
#include "esp_wifi.h"

#define AP_PER_PAGE 6

char displayBuffer[100] = {0};
unsigned long lastDisplayUpdateTime = 0;
uint16_t ap_current_page = 1;
uint8_t ap_max_pages = 4;

NexText iTempSign = NexText(0, 255, "iTempSign");
NexText iTemp = NexText(0, 255, "iTemp");
NexText iTempTrend = NexText(0, 255, "iTempTrend");
NexText iTempFract = NexText(0, 255, "iTempFract");
NexText iTempMin = NexText(0, 255, "iTempMin");
NexText iTempMax = NexText(0, 255, "iTempMax");
NexText iDewPoint = NexText(0, 255, "iDewPoint");
NexText iHumIndex = NexText(0, 255, "iHumIndex");
NexText iHum = NexText(0, 255, "iHum");
NexText iHumTrend = NexText(0, 255, "iHumTrend");
NexText iHumMin = NexText(0, 255, "iHumMin");
NexText iHumMax = NexText(0, 255, "iHumMax");

NexText oTempSign = NexText(0, 255, "oTempSign");
NexText oTemp = NexText(0, 255, "oTemp");
NexText oTempTrend = NexText(0, 255, "oTempTrend");
NexText oTempFract = NexText(0, 255, "oTempFract");
NexText oTempMin = NexText(0, 255, "oTempMin");
NexText oTempMax = NexText(0, 255, "oTempMax");
NexText oDewPoint = NexText(0, 255, "oDewPoint");
NexText oHumIndex = NexText(0, 255, "oHumIndex");
NexText oHum = NexText(0, 255, "oHum");
NexText oHumTrend = NexText(0, 255, "oHumTrend");
NexText oHumMin = NexText(0, 255, "oHumMin");
NexText oHumMax = NexText(0, 255, "oHumMax");

NexHotspot oHot1 = NexHotspot(0, 61, "oHot1");
NexHotspot oHot2 = NexHotspot(0, 62, "oHot2");
NexText oSensIdx = NexText(0, 255, "oSensIdx");
NexText oSignal = NexText(0, 255, "oSignal");
NexText oBatt = NexText(0, 255, "oBatt");
NexPicture forecastImg = NexPicture(0, 16, "forecastImg");
NexText forecast = NexText(0, 5, "forecast");
NexText co2 = NexText(0, 255, "co2");
NexText co2Trend = NexText(0, 255, "co2Trend");
NexText pressure = NexText(0, 255, "pressure");
NexText pressureTrend = NexText(0, 255, "pressureTrend");

NexText dateText = NexText(0, 255, "date");
NexText hourText = NexText(0, 255, "hour");
NexText minuteText = NexText(0, 255, "minute");

NexText wifiSignal = NexText(0, 60, "wifiSignal");

/* Page WiFi config */
NexText wifiSSID = NexText(2, 2, "ssid");
NexText wifiPass = NexText(2, 4, "pass");
NexText wifiIP = NexText(2, 7, "ip");
NexText wifiNetmask = NexText(2, 6, "netmask");
NexText wifiGW = NexText(2, 9, "gw");
NexButton wifiScan = NexButton(2, 18, "scanWifi");
NexButton wifiSave = NexButton(2, 17, "wifiSave");
NexNumber wifiStaticIp = NexNumber(2, 11, "rs");

/* Page WiFi config */
NexButton wifiScanPrev = NexButton(5, 1, "prevB");
NexButton wifiScanNext = NexButton(5, 2, "nextB");
NexButton wifiScanRescan = NexButton(5, 25, "scanB");

NexTouch *nextionListen[] = {&oHot1, &oHot2, &wifiSignal, &wifiScan, &wifiScanPrev, &wifiScanNext, &wifiScanRescan,
                             &wifiSave};

static uint32_t getCO2Color(int16_t co2)
{
    if (co2 > 0 && co2 <= 600)
    {
        return 960;
    } else if (co2 > 600 && co2 <= 800)
    {
        return 4800;
    } else if (co2 > 800 && co2 <= 1000)
    {
        return 33895;
    } else if (co2 > 1000 && co2 <= 2000)
    {
        return 39622;
    } else if (co2 > 2000 && co2 <= 5000)
    {
        return 39235;
    } else if (co2 > 5000)
    {
        return 57376;
    }
}

static uint32_t getHumindexColor(int humindex)
{
    if (humindex <= 19)
    {
        return 1024;
    }
    if (humindex >= 20 && humindex <= 29)
    {
        return 832;
    } else if (humindex >= 30 && humindex <= 39)
    {
        return 50500;
    } else if (humindex >= 40 && humindex <= 45)
    {
        return 45828;
    } else if (humindex > 45)
    {
        return 55554;
    }

    return 0;
}

static void printCurrentOutdoorSensor()
{
    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%i", currentOutdoorSensorId + 1);
    oSensIdx.setText(displayBuffer);

    switch (externalSensorData[currentOutdoorSensorId].signal)
    {
        case 1:
            oSignal.setText("5"); // Full signal
            break;
        case 0:
            oSignal.setText("3"); // Low signal
            break;
        default:
            oSignal.setText("0"); // No signal
            break;
    }

    if (externalSensorData[currentOutdoorSensorId].battery == 255)
    {
        oBatt.setText("0"); // No battery
    } else if (externalSensorData[currentOutdoorSensorId].battery >= 76)
    {
        oBatt.setText("5"); // Full battery
    } else if (externalSensorData[currentOutdoorSensorId].battery >= 51 &&
               externalSensorData[currentOutdoorSensorId].battery <= 75)
    {
        oBatt.setText("4"); // Good battery
    } else if (externalSensorData[currentOutdoorSensorId].battery >= 26 &&
               externalSensorData[currentOutdoorSensorId].battery <= 50)
    {
        oBatt.setText("3"); // Half battery
    } else if (externalSensorData[currentOutdoorSensorId].battery >= 6 &&
               externalSensorData[currentOutdoorSensorId].battery <= 25)
    {
        oBatt.setText("2"); // Low battery
    } else if (externalSensorData[currentOutdoorSensorId].battery >= 0 &&
               externalSensorData[currentOutdoorSensorId].battery <= 5)
    {
        oBatt.setText("1"); // Critical battery
    }

    oTempSign.setText(externalSensorData[currentOutdoorSensorId].temperature < 0 ? "-" : " ");

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", getIntegerFromFloat(externalSensorData[currentOutdoorSensorId].temperature));
    oTemp.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, ".%d", getFractionFromFloat(externalSensorData[currentOutdoorSensorId].temperature));
    oTempFract.setText(displayBuffer);

    switch (getTrend(externalTemperatureLastHour[currentOutdoorSensorId], 0, 60))
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

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) externalSensorData[currentOutdoorSensorId].temperatureMin);
    oTempMin.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) externalSensorData[currentOutdoorSensorId].temperatureMax);
    oTempMax.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) externalSensorData[currentOutdoorSensorId].dewPoint);
    oDewPoint.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) externalSensorData[currentOutdoorSensorId].humIndex);
    oHumIndex.setText(displayBuffer);
    oHumIndex.setFont(getHumindexColor((int) externalSensorData[currentOutdoorSensorId].humIndex));

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", getIntegerFromFloat(externalSensorData[currentOutdoorSensorId].humidity));
    oHum.setText(displayBuffer);

    switch (getTrend(externalHumidityLastHour[currentOutdoorSensorId], 0, 60))
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

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", getIntegerFromFloat(externalSensorData[currentOutdoorSensorId].humidityMin));
    oHumMin.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", getIntegerFromFloat(externalSensorData[currentOutdoorSensorId].humidityMax));
    oHumMax.setText(displayBuffer);
}

static void printTime()
{
    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", hour());
    hourText.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", minute());
    minuteText.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02u.%02u.%02u", day(), month(), year() - 2000);
    dateText.setText(displayBuffer);
}

static void printIndoorSensor()
{
    iTempSign.setText(internalSensorData.temperature < 0 ? "-" : " ");

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", getIntegerFromFloat(internalSensorData.temperature));
    iTemp.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, ".%d", getFractionFromFloat(internalSensorData.temperature));
    iTempFract.setText(displayBuffer);

    switch (getTrend(temperatureLastHour, 0, 60))
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

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) internalSensorData.temperatureMin);
    iTempMin.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) internalSensorData.temperatureMax);
    iTempMax.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) internalSensorData.dewPoint);
    iDewPoint.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) internalSensorData.humIndex);
    iHumIndex.setText(displayBuffer);
    iHumIndex.setFont(getHumindexColor((int) internalSensorData.humIndex));

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", (int) abs(internalSensorData.humidity));
    iHum.setText(displayBuffer);

    switch (getTrend(humidityLastHour, 0, 60))
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

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", getIntegerFromFloat(internalSensorData.humidityMin));
    iHumMin.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%02d", getIntegerFromFloat(internalSensorData.humidityMax));
    iHumMax.setText(displayBuffer);

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%i", internalSensorData.co2);
    co2.setText(displayBuffer);
    co2.setFont(getCO2Color(internalSensorData.co2));

    switch (getTrend(co2LastHour, 0, 3))
    {
        case T_RISING:
            co2Trend.setText("8");
            break;
        case T_FALLING:
            co2Trend.setText("9");
            break;
        default:
            co2Trend.setText(":");
            break;
    }

    displayBuffer[0] = '\0';
    sprintf(displayBuffer, "%i", internalSensorData.pressureMmHg);
    pressure.setText(displayBuffer);

    if (0 == pressureLast24H[21])
    {
        pressureTrend.setText(":");
    } else
    {
        // Trend for last 3h
        switch (getTrend(pressureLast24H, 21, 3))
        {
            case T_RISING:
                pressureTrend.setText("8");
                break;
            case T_FALLING:
                pressureTrend.setText("9");
                break;
            default:
                pressureTrend.setText(":");
                break;
        }
    }
}

static void printWifiStatus()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        wifiSignal.setText("0");
        wifiSignal.Set_font_color_pco(57376); //Red
    } else
    {
        int8_t rssi = WiFi.RSSI();
        if (rssi <= -85)
        {
            wifiSignal.setText("1");
            wifiSignal.Set_font_color_pco(65535); // White
        } else if (rssi > -85 && rssi <= -67)
        {
            wifiSignal.setText("2");
            wifiSignal.Set_font_color_pco(65535); // White
        } else if (rssi > -67)
        {
            wifiSignal.setText("3");
            wifiSignal.Set_font_color_pco(65535); // White
        }
    }
}

static void printAccessPoints(uint8_t page)
{
    int16_t scanCount = WiFi.scanComplete();

    if (scanCount <= 0)
    {
        return;
    }

    ap_max_pages = scanCount / AP_PER_PAGE;
    if (scanCount % AP_PER_PAGE > 0)
    {
        ap_max_pages++;
    }

    page = min((int16_t) max((int16_t) page, (int16_t) 1), scanCount);
    uint16_t from = page > 1 ? (page - 1) * AP_PER_PAGE : 0;
    uint16_t to = min((int16_t)(from + AP_PER_PAGE), scanCount);

    ESP_LOGI(TAG, "Page: %i, From: %i, To: %i", page, from, to);
    ESP_LOGI(TAG, "Total APs scanned = %u", scanCount);
    ESP_LOGI(TAG, "Total pages = %u", ap_max_pages);

    uint8_t field_index = 0;
    for (int i = from; i < to; i++)
    {
        char ssid[64] = {0};
        WiFi.SSID(i).toCharArray(ssid, 64);

        ESP_LOGI(
                TAG,
                "%i: %s (%i) %s",
                i + 1,
                ssid,
                WiFi.RSSI(i),
                (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*"
        );

        field_index++;
        char ssid_field[30];
        sprintf(ssid_field, "s%i.txt=\"%s\"", field_index, ssid);
        sendCommand(ssid_field);

        char auth_field[15];
        sprintf(auth_field, "l%i.txt=\"%s\"", field_index, (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "" : "0");
        sendCommand(auth_field);

        char rssi_field[15];
        sprintf(rssi_field, "r%i.txt=\"%i\"", field_index, WiFi.RSSI(i));
        sendCommand(rssi_field);
    }

    sendCommand("vis scanB,1");

    if (ap_max_pages > 1)
    {
        if (ap_current_page < ap_max_pages)
        {
            sendCommand("vis nextB,1");
        } else
        {
            sendCommand("vis nextB,0");
        }

        if (ap_current_page > 1)
        {
            sendCommand("vis prevB,1");
        } else
        {
            sendCommand("vis prevB,0");
        }
    }
}

static void printForecast()
{
    if (strcmp(weather_condition, "clear-night") == 0 || strcmp(weather_condition, "sunny") == 0)
    {
        forecastImg.setPic(1);
    } else if (strcmp(weather_condition, "cloudy") == 0 || strcmp(weather_condition, "windy-variant") == 0 ||
               strcmp(weather_condition, "fog") == 0 || strcmp(weather_condition, "windy") == 0)
    {
        forecastImg.setPic(8);
    } else if (strcmp(weather_condition, "hail") == 0 || strcmp(weather_condition, "pouring") == 0)
    {
        forecastImg.setPic(5);
    } else if (strcmp(weather_condition, "rainy") == 0)
    {
        forecastImg.setPic(2);
    } else if (strcmp(weather_condition, "snowy") == 0 || strcmp(weather_condition, "snowy-rainy") == 0)
    {
        forecastImg.setPic(6);
    } else if (strcmp(weather_condition, "lightning") == 0 || strcmp(weather_condition, "lightning-rainy") == 0)
    {
        forecastImg.setPic(7);
    } else if (strcmp(weather_condition, "partlycloudy") == 0)
    {
        forecastImg.setPic(8);
    } else
    {
        forecastImg.setPic(9);
    }

    char condition[20];
    strcpy(condition, weather_condition);
    condition[0] = toupper(condition[0]);
    forecast.setText(condition);
}

void initDisplay()
{
    nexInit();
    oHot1.attachPush(oHot1PushCallback, &oHot1);
    oHot2.attachPush(oHot2PushCallback, &oHot2);
    wifiSignal.attachPop(wifiSignalPopCallback, &wifiSignal);
    wifiScan.attachPop(wifiScanPopCallback, &wifiScan);
    wifiScanPrev.attachPop(wifiScanPrevPopCallback, &wifiScanPrev);
    wifiScanNext.attachPop(wifiScanNextPopCallback, &wifiScanNext);
    wifiScanRescan.attachPop(wifiScanPopCallback, &wifiScanRescan);
    wifiSave.attachPop(wifiSavePopCallback, &wifiSave);

    sendCommand("page 0");
}

void oHot1PushCallback(void *ptr)
{
    if (currentOutdoorSensorId == 0)
    {
        currentOutdoorSensorId = RF_SENSORS_COUNT - 1;
    } else
    {
        currentOutdoorSensorId--;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(currentOutdoorSensorId + 1, displayBuffer, 10);

    printCurrentOutdoorSensor();
}

void oHot2PushCallback(void *ptr)
{
    currentOutdoorSensorId++;
    if (currentOutdoorSensorId >= RF_SENSORS_COUNT)
    {
        currentOutdoorSensorId = 0;
    }

    memset(displayBuffer, 0, sizeof(displayBuffer));
    itoa(currentOutdoorSensorId + 1, displayBuffer, 10);

    printCurrentOutdoorSensor();
}

void wifiSignalPopCallback(void *ptr)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        wifi_config_t wifi_config;
        esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
        tcpip_adapter_ip_info_t ipinfo;
        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipinfo);

        char ip[18];
        char netmask[18];
        char gw[18];
        sprintf(ip, IPSTR, IP2STR(&ipinfo.ip));
        sprintf(netmask, IPSTR, IP2STR(&ipinfo.netmask));
        sprintf(gw, IPSTR, IP2STR(&ipinfo.gw));

        wifiSSID.setText((char *) wifi_config.sta.ssid);
        wifiPass.setText((char *) wifi_config.sta.password);
        wifiIP.setText(ip);
        wifiNetmask.setText(netmask);
        wifiGW.setText(gw);
    }
}

void wifiScanPopCallback(void *ptr)
{
    ESP_LOGI(TAG, "Start scanning WiFI");
    ESP_LOGI(TAG, "Disconnecting...");

    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);
    WiFi.disconnect();
    delay(100);

    ESP_LOGI(TAG, "Disconnected");

    sendCommand("vis loader,1");
    sendCommand("vis scanB,0");
    sendCommand("vis prevB,0");
    sendCommand("vis nextB,0");

    int n = WiFi.scanNetworks();
    ESP_LOGI(TAG, "Scan done");

    sendCommand("vis loader,0");

    if (n <= 0)
    {
        ESP_LOGI(TAG, "No networks found");
    } else
    {
        ESP_LOGI(TAG, "%i networks found", n);

        ap_current_page = 1;
        printAccessPoints(ap_current_page);
    }
}

void wifiScanPrevPopCallback(void *ptr)
{
    ap_current_page--;
    if (ap_current_page < 1)
    {
        ap_current_page = 1;
    }

    printAccessPoints(ap_current_page);
}

void wifiScanNextPopCallback(void *ptr)
{
    ap_current_page++;
    if (ap_current_page > ap_max_pages)
    {
        ap_current_page = ap_max_pages;
    }

    printAccessPoints(ap_current_page);
}

void wifiSavePopCallback(void *ptr)
{
    char ssid[64] = {0};
    char pass[64] = {0};
    char ip[24] = {0};
    char netmask[24] = {0};
    char gw[24] = {0};
    uint32_t isStaticIp = 0;

    wifiStaticIp.getValue(&isStaticIp);
    wifiSSID.getText(ssid, 64);
    wifiPass.getText(pass, 64);

    WiFi.disconnect();

    if (isStaticIp == 1)
    {
        wifiIP.getText(ip, 24);
        wifiNetmask.getText(netmask, 24);
        wifiGW.getText(gw, 24);

        IPAddress local_IP;
        local_IP.fromString(ip);

        IPAddress gateway;
        gateway.fromString(gw);

        IPAddress subnet;
        subnet.fromString(netmask);

        ESP_LOGI(TAG, "\nSSID: %s\nPass: %s\nIP: %s\nMask: %s\nGW: %s", ssid, pass, ip, netmask, gw);

        if (!WiFi.config(local_IP, gateway, subnet))
        {
            ESP_LOGE(TAG, "STA Failed to configure static IP");
        }
    } else
    {
        ESP_LOGI(TAG, "\nSSID: %s\nPass: %s", ssid, pass);
    }

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid);

    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, pass);

    sendCommand("page 0");
}

void redrawDisplay()
{
    if ((millis() - lastDisplayUpdateTime > UPDATE_DISPLAY_INTERVAL))
    {
        lastDisplayUpdateTime = millis();

        printWifiStatus();
        printIndoorSensor();
        printTime();
        printCurrentOutdoorSensor();
        printForecast();
    }
}
