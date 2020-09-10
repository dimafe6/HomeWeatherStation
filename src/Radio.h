#ifndef RADIO_H
#define RADIO_H

#include "Arduino.h"
#include "Audio.h"
#include "Config.h"

extern uint8_t radioVolume;

void startRadio();
void radioTaskCode(void *pvParameters);

#endif