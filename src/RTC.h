#ifndef RTC_H
#define RTC_H

#include "Arduino.h"
#include <RtcDS3231.h>
#include <TimeLib.h>
#include "Config.h"
#include <Wire.h>
#include "Utils.h"

extern RtcDS3231<TwoWire> Rtc;

void initRtc();
void printDateTime(const RtcDateTime &dt);

#endif