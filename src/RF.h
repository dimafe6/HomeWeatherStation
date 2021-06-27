#ifndef RF_H
#define RF_H

#include "Arduino.h"
#include <RF24.h>
#include "Globals.h"
#include "Config.h"
#include "RTC.h"

void initRF();
void nrf24_task();

#endif