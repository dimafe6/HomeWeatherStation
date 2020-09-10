#include "Radio.h"

Audio audio;
TaskHandle_t radioTask;

uint8_t radioVolume = 4;

void startRadio()
{
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(radioVolume);
    audio.connecttohost("http://online.radioroks.ua/RadioROKS_HD");

    xTaskCreatePinnedToCore(
        radioTaskCode,
        "Radio",
        40000,
        NULL,
        1,
        &radioTask,
        0);
}

void radioTaskCode(void *pvParameters)
{
    for (;;)
    {
        audio.loop();
    }
}