#include "RTC.h"

static const char *TAG = "BME280";

RtcDS3231 <TwoWire> Rtc(Wire);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

TimeChangeRule myDST = {"EEST", Last, Sun, Mar, 3, 180};
TimeChangeRule mySTD = {"EET", Last, Sun, Oct, 4, 120};
Timezone uaTZ(myDST, mySTD);

void initRtc()
{
    pinMode(RF_SCLK, INPUT_PULLUP);
    pinMode(RF_MISO, INPUT_PULLUP);
    pinMode(RF_MOSI, INPUT_PULLUP);
    pinMode(RTC_IRQ_PIN, INPUT);

    Rtc.Begin();

    if (!Rtc.GetIsRunning())
    {
        ESP_LOGI(TAG, "RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    setSyncProvider(rtcEpoch32Time);

    Rtc.Enable32kHzPin(false);

    ESP_LOGI(TAG, "RTC module time:");
    printDateTime(Rtc.GetDateTime());
}

void printDateTime(const RtcDateTime &dt)
{
    ESP_LOGI(
            TAG,
            "%04u/%02u/%02u %02u:%02u:%02u",
            dt.Year(),
            dt.Month(),
            dt.Day(),
            dt.Hour(),
            dt.Minute(),
            dt.Second()
    );
}

void syncTimeFromNTP()
{
    ESP_LOGI(TAG, "Sync time from NTP");

    timeClient.begin();
    timeClient.forceUpdate();
    timeClient.end();

    setTime(uaTZ.toLocal(timeClient.getEpochTime()));
    Rtc.SetDateTime(RtcDateTime(year(), month(), day(), hour(), minute(), second()));

    ESP_LOGI(TAG, "Time from NTP:");
    printDateTime(Rtc.GetDateTime());
}

time_t rtcEpoch32Time()
{
    return Rtc.GetDateTime().Epoch32Time();
}