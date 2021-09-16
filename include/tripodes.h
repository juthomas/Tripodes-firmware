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

#define SCALE_DFA 1
#define SCALE_DFA_TRESHOLD 3

#define TRIPODE_ID "1_1"
// #define FRACTAL_STATE_POS "10;10"
#define FRACTAL_STATE_POS_X 0
// #define FRACTAL_STATE_POS_X 12
// #define FRACTAL_STATE_POS_X 24
#define FRACTAL_STATE_POS_Y 10
#define UDP_DRAWING 1
#define UPD_INIT_ORCA 1
// #define UPD_INIT_ORCA 1
#define GLYPH_X_POS 48

#define GLYPH_Y_POS 6
// #define GLYPH_Y_POS 35
// #define GLYPH_Y_POS 64
//29

//4 => 140
//3 => 150

const IPAddress local_IP(10, 87, 210, 202);


const uint16_t updMessageRate = 60; // 1/Rate => 20 = 1bang/2secs
const uint16_t updDrawRate = 300; // 1/Rate => 20 = 1bang/2secs

// const IPAddress KooOutIp(10,0,1,14);

//1  2  3  4
//6, 7, 5, 12
// const IPAddress Rasp1OutIp(10,87,210,255);   // Raspi4 local addr
// // const IPAddress Rasp1OutIp(10,87,210,255);   // Raspi4 local addr
// // const IPAddress Rasp1OutIp(10,87,208,68);   // Raspi4 local addr
// const unsigned int Rasp1OutPort = 49160;       // Orca input port

float	dfa(float *x, size_t size_x, float min_scale, float max_scale, float scale_dens);

typedef struct	s_sta_list
{
	String		ip_adress;
	bool		has_website;
}				t_sta_list;

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

void drawUpdSendingActivity(TFT_eSprite *sprite);

void drawCursors(TFT_eSprite *sprite, int x, int y, int w, int h, int min, int max, int value, uint32_t color);

void drawBatteryLevel(TFT_eSprite *sprite, int x, int y, float voltage);


void drawMotorsActivity(TFT_eSPI tft, int32_t pwmValues[3], int32_t localUdpPort, const char *ssid, bool is_upd_sending);

void drawSensorsActivity(TFT_eSPI tft, t_sensors sensors, int32_t oscAddress, bool is_upd_sending);

void drawAlpha(TFT_eSPI tft, float alpha, bool is_upd_sending);

//dfa.cpp
float mean(float *tab, size_t tab_size);


void	web();



#endif