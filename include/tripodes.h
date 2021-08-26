#ifndef TRIPODES_H
# define TRIPODES_H
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"
#include <WiFiUdp.h>
#include "esp_wifi.h"
#include <Adafruit_Sensor.h>
#include <L3G.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <EEPROM.h>
#include <OSCMessage.h>
#include <string>  
#include <sstream>
#define EEPROM_SIZE 512

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23

#define TFT_BL 4  // Display backlight control pin
#define ADC_EN 14 //ADC_EN is the ADC detection enable port
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

#define MOTOR_1 25
#define MOTOR_2 26
#define MOTOR_3 27

//VIN 3Vo GND SCL SDA GINT GRDY LIN1 LIN2 LRDY

#define SDA 21
#define SCL 22

#define VREF 1100


float	dfa(float *x, size_t size_x, float min_scale, float max_scale, float scale_dens);

typedef struct	s_float3
{
	float		x;
	float		y;
	float		z;
}				t_float3;

typedef struct	s_sensors{
	t_float3	accel;
	t_float3	gyro;
	t_float3	mag;
}				t_sensors;

// main.c
double fmap(double x, double in_min, double in_max, double out_min, double out_max);


// ui.c
void compassArraw(TFT_eSPI tft, TFT_eSprite * sprite, int x, int y, float angle);


void drawCursors(TFT_eSprite *sprite, int x, int y, int w, int h, int min, int max, int value, uint32_t color);

void drawBatteryLevel(TFT_eSprite *sprite, int x, int y, float voltage);

void drawMotorsActivity(TFT_eSPI tft, int32_t pwmValues[3], int32_t localUdpPort, const char *ssid);

void drawSensorsActivity(TFT_eSPI tft, t_sensors sensors, int32_t oscAddress);

void drawAlpha(TFT_eSPI tft, t_sensors sensors);

#endif