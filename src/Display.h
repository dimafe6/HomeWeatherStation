#ifndef DISPLAY_H
#define DISPLAY_H

#include "Arduino.h"
#include "Config.h"
#include "Globals.h"
#include "Utils.h"
#include "./Nextion/Nextion.h"

extern NexTouch *nextionListen[];

void oHotPopCallback(void *ptr);
void initDisplay();
void redrawDisplay(bool force);

#endif