#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MQTT_SERVER "192.168.1.31"
#define MQTT_USER ""
#define MQTT_PASS ""
#define MQTT_CLIENT_NAME "HomeWeatherStation"
#define MQTT_PORT 1883
#define MQTT_SEND_INTERVAL 10000

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
#define UPDATE_BH1750_INTERVAL 1000
#define UPDATE_BME280_INTERVAL 5000
#define UPDATE_MHZ19_INTERVAL 10000
#define UPDATE_SENSORS_INTERVAL 5000
#define RF_SENSORS_COUNT 5
#define RF_MAX_LOSSS_MESSAGES_BEFORE_LOSE_SIGNAL 5
#define UPDATE_DISPLAY_INTERVAL UPDATE_SENSORS_INTERVAL
#define NTP_SYNC_TIME_INTERVAL 60000


#endif