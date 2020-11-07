#include "Arduino.h"
#include "./src/Globals.h"
#include "./src/Nextion/Nextion.h"
#include "./src/Sensors/Sensors.h"
#include "./src/RF.h"
#include "./src/RTC.h"
#include "./src/Server.h"
#include "./src/Radio.h"
#include "./src/Display.h"
#include "./src/WiFi.h"

void setup() {
    Serial.begin(9600);
    initDisplay();
    initRtc();
    initRF();
    initSPIFFS();
    initBME280();
    initMHZ19();
    initLightMeter();
    initWiFi();
    initWebServer(); 
    //startRadio();
}

void loop() {
  if (shouldReboot)
  {
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }

  readExternalSensorData();
  readAllSensors(false);

  checkSignal();

  nexLoop(nextionListen);
  redrawDisplay(false);
}

void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");
  }

  Serial.println("");
  Serial.print("SPIFFS total:");
  Serial.println(SPIFFS.totalBytes());
  Serial.println("");
  Serial.print("SPIFFS used:");
  Serial.println(SPIFFS.usedBytes());
}

