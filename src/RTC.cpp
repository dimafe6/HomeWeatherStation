#include "RTC.h"

RtcDS3231<TwoWire> Rtc(Wire);
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
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    setSyncProvider(rtcEpoch32Time);

    Rtc.Enable32kHzPin(false);

    Serial.println("\nRTC module time:");
    printDateTime(Rtc.GetDateTime());
}

void printDateTime(const RtcDateTime &dt)
{
    char datestring[20];

    snprintf_P(datestring,
               countof(datestring),
               PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
               dt.Month(),
               dt.Day(),
               dt.Year(),
               dt.Hour(),
               dt.Minute(),
               dt.Second());
    Serial.print(datestring);
}

void syncTimeFromNTP()
{
    timeClient.begin();
    timeClient.forceUpdate();
    timeClient.end();

    setTime(uaTZ.toLocal(timeClient.getEpochTime()));
    Rtc.SetDateTime(RtcDateTime(year(), month(), day(), hour(), minute(), second()));

    Serial.println("RTC module time:");
    printDateTime(Rtc.GetDateTime());
}

time_t rtcEpoch32Time()
{
    return Rtc.GetDateTime().Epoch32Time();
}

void syncTimeIfRTCInvalid()
{
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    RtcDateTime now = Rtc.GetDateTime();

    if (!Rtc.IsDateTimeValid() || now < compiled)
    {
        if (Rtc.LastError() != 0)
        {
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }

        Serial.println("RTC module time is invalid. Try to sync with NTP...");

        syncTimeFromNTP();
    }
}