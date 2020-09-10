#ifndef SERVER_H
#define SERVER_H

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <Update.h>
#include "FS.h"
#include "SPIFFS.h"
#include "Secrets.h"
#include "Globals.h"

void initWebServer();

#endif