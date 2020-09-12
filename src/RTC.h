#ifndef RTC_H
#define RTC_H

#include "Arduino.h"
#include <RtcDS3231.h>
#include <TimeLib.h>
#include "Config.h"
#include <Wire.h>
#include "Utils.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h>

void initRtc();
void printDateTime(const RtcDateTime &dt);
void syncTimeFromNTP();
void syncTimeIfRTCInvalid();
time_t rtcEpoch32Time();

#endif