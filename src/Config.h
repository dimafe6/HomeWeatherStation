#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Pins */
#define MH_Z19_ABC_INTERVAL_HOURS 24

#define RTC_IRQ_PIN 36

#define RF_CE 2
#define RF_CSN 4
#define RF_MISO 19
#define RF_MOSI 23
#define RF_SCLK 18
#define RF_IRQ 34
#define RX1 33
#define TX1 32
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

#define PIR_IRQ 35

/* Other configs */
#define UPDATE_SENSORS_INTERVAL 5000 //20sec
#define SEND_SENSORS_INTERVAL 600000
#define RF_SENSORS_COUNT 5
#define PRESSURE_HISTORY_INTERVAL 3600000 //3600000 = 1 hour
#define TEMPERATURE_HISTORY_INTERVAL 900000 //900000 = 15 min
#define HUMIDITY_HISTORY_INTERVAL 900000 //900000 = 15 min
#define CO2_HISTORY_INTERVAL 900000 //900000 = 15 min
#define EXTERNAL_TEMPERATURE_HISTORY_INTERVAL 900000 //900000 = 15 min
#define EXTERNAL_HUMIDITY_HISTORY_INTERVAL 900000 //900000 = 15 min
#define UPDATE_DISPLAY_INTERVAL 5000

/* Web server configuration */
#define HTTP_PORT 80

#endif