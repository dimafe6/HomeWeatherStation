#include "RTC.h"

RtcDS3231<TwoWire> Rtc(Wire);

void initRtc()
{
    pinMode(RF_SCLK, INPUT_PULLUP);
    pinMode(RF_MISO, INPUT_PULLUP);
    pinMode(RF_MOSI, INPUT_PULLUP);
    pinMode(RTC_IRQ_PIN, INPUT);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    if (!Rtc.IsDateTimeValid())
    {
        if (Rtc.LastError() != 0)
        {
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            Serial.println("RTC lost confidence in the DateTime!");
            Rtc.SetDateTime(compiled);
        }
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    setTime(now.Epoch32Time());

    if (now < compiled)
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled)
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled)
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    Rtc.Enable32kHzPin(false);

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