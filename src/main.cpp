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
// #include <Adafruit_L3GD20_U.h>

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

L3G gyro;

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303DLH_Mag_Unified mag = Adafruit_LSM303DLH_Mag_Unified(12345);
// Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
const int motorFreq = 5000;
const int motorResolution = 8;
const int motorChannel1 = 0;
const int motorChannel2 = 1;
const int motorChannel3 = 2;

typedef struct s_data_task
{
	int duration;
	int pwm;
	int motor_id;
	TaskHandle_t thisTaskHandler;
} t_data_task;

// typedef void(*t_task_func)(void *param);

enum e_wifi_modes
{
	NONE_MODE = 0,
	STA_MODE,
	AP_MODE,
	GYRO_MODE
};

t_data_task g_data_task[3];

int vref = 1100;
int timers_end[3] = {0, 0, 0};

#define BLACK 0x0000
#define WHITE 0xFFFF
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

e_wifi_modes current_mode;
// const char* ssid = "Freebox-0E3EAE";
// const char* password =  "taigaest1chien";

// const char *ssid = "Dourr";
// const char *password = "Akiraestlepluscooldeschien28os";

const char *ssid = "tripodesAP";
const char *password = "44448888";

const char *APssid = "tripodesAP";
const char *APpassword = "44448888";

WiFiUDP Udp;

const IPAddress outIp(192,168,0,12);        // remote IP of your computer
// const IPAddress outIp(192,168,0,41);        // remote IP of your computer

// const IPAddress outIp(192,168,56,1);        // remote IP of your computer
const unsigned int outPort = 2002;          // remote port to receive OSC




bool oscAddressChanged = false;
unsigned int oscAddress = 1;
unsigned int outUdpPort = 49100;
unsigned int localUdpPort = 49141;
char incomingPacket[255];
String convertedPacket;
char replyPacket[] = "Message received";

void set_pwm0(int pwm);
void set_pwm1(int pwm);
void set_pwm2(int pwm);

typedef void (*t_set_pwm)(int pwm);

#define TASK_NUMBER 3
static const t_set_pwm g_set_pwm[TASK_NUMBER] = {
	(t_set_pwm)set_pwm0,
	(t_set_pwm)set_pwm1,
	(t_set_pwm)set_pwm2,
};

void stop_pwm0(void);
void stop_pwm1(void);
void stop_pwm2(void);

typedef void (*t_stop_pwm)(void);

#define TASK_NUMBER 3
static const t_stop_pwm g_stop_pwm[TASK_NUMBER] = {
	(t_stop_pwm)stop_pwm0,
	(t_stop_pwm)stop_pwm1,
	(t_stop_pwm)stop_pwm2,
};

WiFiClass WiFiAP;

bool timersActives[3];
int pwmValues[3];
int timerPansements[3];
hw_timer_t *timers[4] = {NULL, NULL, NULL, NULL};

const char *wl_status_to_string(int ah)
{
	switch (ah)
	{
	case WL_NO_SHIELD:
		return "WL_NO_SHIELD";
	case WL_IDLE_STATUS:
		return "WL_IDLE_STATUS";
	case WL_NO_SSID_AVAIL:
		return "WL_NO_SSID_AVAIL";
	case WL_SCAN_COMPLETED:
		return "WL_SCAN_COMPLETED";
	case WL_CONNECTED:
		return "WL_CONNECTED";
	case WL_CONNECT_FAILED:
		return "WL_CONNECT_FAILED";
	case WL_CONNECTION_LOST:
		return "WL_CONNECTION_LOST";
	case WL_DISCONNECTED:
		return "WL_DISCONNECTED";
	default:
		return "ERROR NOT VALID WL";
	}
}

const char *eTaskGetState_to_string(int ah)
{
	switch (ah)
	{
	case eRunning:
		return "eRunning";
	case eReady:
		return "eReady";
	case eBlocked:
		return "eBlocked";
	case eSuspended:
		return "eSuspended";
	case eDeleted:
		return "eDeleted";
	default:
		return "ERROR NOT STATE";
	}
}

double fmap(double x, double in_min, double in_max, double out_min, double out_max) {
    const double dividend = out_max - out_min;
    const double divisor = in_max - in_min;
    const double delta = x - in_min;
    if(divisor == 0){
        log_e("Invalid map input range, min == max");
        return -1; //AVR returns -1, SAM returns 0
    }
    return (delta * dividend + (divisor / 2.0)) / divisor + out_min;
}




void IRAM_ATTR button1_handler(Button2 &btn)
{
	uint32_t click_type = btn.getClickType();

	if (click_type == SINGLE_CLICK || click_type == LONG_CLICK)
	{
		Serial.println("Bouton A pressed");
		if (current_mode == NONE_MODE)
		{
			current_mode = STA_MODE;
		}
		else if (current_mode == GYRO_MODE)
		{
			oscAddress = oscAddress < 99999 ? oscAddress + 1 : 99999;
			oscAddressChanged = true;
			// EEPROM.writeUInt(10, oscAddress);
			// EEPROM.commit();
		}
	}
}

void IRAM_ATTR button2_handler(Button2 &btn)
{
	uint32_t click_type = btn.getClickType();

	if (click_type == SINGLE_CLICK || click_type == LONG_CLICK)
	{
		Serial.println("Bouton B pressed");
		if (current_mode == NONE_MODE)
		{
			current_mode = AP_MODE;
		}
		else if (current_mode == GYRO_MODE)
		{
			oscAddress = oscAddress > 0 ? oscAddress - 1 : 0;
			oscAddressChanged = true;
			// EEPROM.writeUInt(10, oscAddress);
			// EEPROM.commit();
		}
	}
	if (click_type == DOUBLE_CLICK)
	{
		Serial.println("Bouton B double clicked");

		// oscAddress = EEPROM.readUInt(10);
		current_mode = GYRO_MODE;
	}
}

void button_init()
{
	btn1.setClickHandler(button1_handler);
	btn1.setLongClickHandler(button1_handler);
	btn1.setDoubleClickHandler(button1_handler);
	btn1.setTripleClickHandler(button1_handler);
	btn2.setClickHandler(button2_handler);
	btn2.setLongClickHandler(button2_handler);
	btn2.setDoubleClickHandler(button2_handler);
	btn2.setTripleClickHandler(button2_handler);
}

void IRAM_ATTR button_loop()
{
	btn1.loop();
	btn2.loop();
}

void IRAM_ATTR call_buttons(void)
{
	button_loop();
}

void showVoltage()
{
	static uint64_t timeStamp = 0;
	if (millis() - timeStamp > 1000)
	{
		timeStamp = millis();
		uint16_t v = analogRead(ADC_PIN);
		float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
		String voltage = "Voltage :" + String(battery_voltage) + "V";
		Serial.println(voltage);
		tft.fillScreen(TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString(voltage, tft.width() / 2, tft.height() / 2);
	}
}

void sta_setup()
{
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	tft.drawString("Connecting", tft.width() / 2, tft.height() / 2);
	uint64_t timeStamp = millis();

	ledcSetup(motorChannel1, motorFreq, motorResolution);
	ledcSetup(motorChannel2, motorFreq, motorResolution);
	ledcSetup(motorChannel3, motorFreq, motorResolution);
	ledcAttachPin(MOTOR_1, motorChannel1);
	ledcAttachPin(MOTOR_2, motorChannel2);
	ledcAttachPin(MOTOR_3, motorChannel3);

	Serial.println("Connecting");
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - timeStamp > 10000)
		{
			ESP.restart();
			tft.fillScreen(TFT_BLACK);
			tft.drawString("Restarting", tft.width() / 2, tft.height() / 2);
		}
		delay(500);
		Serial.println(wl_status_to_string(WiFi.status()));
		tft.fillScreen(TFT_BLACK);

		tft.drawString(wl_status_to_string(WiFi.status()), tft.width() / 2, tft.height() / 2);
	}
	Serial.print("Connected, IP address: ");
	Serial.println(WiFi.localIP());
	Udp.begin(localUdpPort);
	Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void ap_setup()
{
	WiFi.mode(WIFI_AP);
	WiFi.softAP(APssid, APpassword, 1, 0, 10);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	tft.printf("AP addr: %s\n", myIP.toString().c_str());
}

void setup()
{
	EEPROM.begin(EEPROM_SIZE);
	// put your setup code here, to run once:
	current_mode = NONE_MODE;
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	Wire.begin();

	oscAddress = EEPROM.readUInt(0);
		// current_mode = GYRO_MODE;



	timers[3] = timerBegin(3, 80, true);
	timerAttachInterrupt(timers[3], &call_buttons, false);
	timerAlarmWrite(timers[3], 50 * 1000, true);
	timerAlarmEnable(timers[3]);
	button_init();

	pinMode(ADC_EN, OUTPUT);
	digitalWrite(ADC_EN, HIGH);
	tft.init();
	tft.setRotation(0);
	tft.fillScreen(TFT_BLACK);

	if (TFT_BL > 0)
	{											// TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
		pinMode(TFT_BL, OUTPUT);				// Set backlight pin to output mode
		digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
	}

	tft.setTextSize(1);
	tft.setTextColor(TFT_WHITE);
	tft.setCursor(0, 0);
	tft.setTextDatum(MC_DATUM);

	tft.printf("Please selected your \nmode \n(with bottom buttons)");
	tft.setCursor(0, 230);
	tft.setTextColor(TFT_RED);
	tft.printf("AP mode");
	tft.setCursor(85, 230);
	tft.setTextColor(TFT_BLUE);
	tft.printf("STA mode");
	tft.setCursor(0, 0);

	for (;;)
	{
		Serial.print("tour de boucle :");
		Serial.println(current_mode);
		if (current_mode == STA_MODE)
		{
			sta_setup();
			break;
		}
		else if (current_mode == AP_MODE)
		{
			ap_setup();
			break;
		}
		else if (current_mode == GYRO_MODE)
		{
				if (!gyro.init())
	{
		Serial.println("Failed to autodetect gyro type!");
		while (1)
			;
	}

	gyro.enableDefault();
	//   gyro.enableAutoRange(true);
	// if(!gyro.begin())
	// {
	// 	/* There was a problem detecting the L3GD20 ... check your connections */
	// 	Serial.println("Ooops, no L3GD20 detected ... Check your wiring!");
	// 	while(1);
	// }

	// gyro.enableDefault();
	// WiFiAP.begin(APssid, APpassword);

	/* Initialise the sensor */
	if (!accel.begin())
	{
		/* There was a problem detecting the ADXL345 ... check your connections */
		Serial.println("Ooops, no LSM303 detected(accel) ... Check your wiring!");
		while (1)
			;
	}

	accel.setRange(LSM303_RANGE_4G);
	accel.setMode(LSM303_MODE_NORMAL);

	/* Enable auto-gain */
	mag.enableAutoRange(true);

	/* Initialise the sensor */
	if (!mag.begin())
	{
		/* There was a problem detecting the LSM303 ... check your connections */
		Serial.println("Ooops, no LSM303 detected(mag) ... Check your wiring!");
		while (1)
			;
	}
			   Udp.begin(localUdpPort);
			break;
		}
		delay(100);
	}
}

void drawBatteryLevel(TFT_eSprite *sprite, int x, int y, float voltage)
{
	uint32_t color1 = TFT_GREEN;
	uint32_t color2 = TFT_WHITE;
	uint32_t color3 = TFT_BLUE;
	uint32_t color4 = TFT_RED;

	if (voltage > 4.33)
	{
		(*sprite).fillRect(x, y, 30, 10, color3);
	}
	else if (voltage > 3.2)
	{
		(*sprite).fillRect(x, y, map((long)(voltage * 100), 320, 430, 0, 30), 10, color1);
		(*sprite).setCursor(x + 7, y + 1);
		(*sprite).setTextColor(TFT_DARKGREY);
		(*sprite).printf("%02ld%%", map((long)(voltage * 100), 320, 432, 0, 100));
	}
	else
	{
		(*sprite).fillRect(x, y, 30, 10, color4);
	}

	(*sprite).drawRect(x, y, 30, 10, color2);
}

void drawMotorsActivity()
{
	TFT_eSprite drawing_sprite = TFT_eSprite(&tft);

	drawing_sprite.setColorDepth(8);
	drawing_sprite.createSprite(tft.width(), tft.height());

	drawing_sprite.fillSprite(TFT_BLACK);
	drawing_sprite.setTextSize(1);
	drawing_sprite.setTextFont(1);
	drawing_sprite.setTextColor(TFT_GREEN);
	drawing_sprite.setTextDatum(MC_DATUM);
	drawing_sprite.setCursor(0, 0);

	uint16_t v = analogRead(ADC_PIN);
	float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Voltage : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%.2fv\n\n", battery_voltage);

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Ssid : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%s\n\n", ssid);

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Ip : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%s\n\n", WiFi.localIP().toString().c_str());

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Udp port : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n\n", localUdpPort);

	// %s\n\nIp : %s\n\nUdp port : %d\n\n", ssid,WiFi.localIP().toString().c_str(), localUdpPort);

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Up time : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%llds\n", esp_timer_get_time() / 1000000);
	drawBatteryLevel(&drawing_sprite, 100, 00, battery_voltage);

	//uint32_t color1 = TFT_BLUE;
	uint32_t color2 = TFT_WHITE;
	drawing_sprite.drawCircle(67, 120, 26, color2);
	drawing_sprite.drawCircle(27, 190, 26, color2);
	drawing_sprite.drawCircle(108, 190, 26, color2);
	drawing_sprite.drawLine(15, 167, 40, 150, color2);
	drawing_sprite.drawLine(40, 150, 40, 120, color2);
	drawing_sprite.drawLine(93, 120, 93, 150, color2);
	drawing_sprite.drawLine(93, 150, 120, 167, color2);
	drawing_sprite.drawLine(100, 215, 67, 195, color2);
	drawing_sprite.drawLine(67, 195, 35, 215, color2);

	//drawing_sprite.drawCircle(TFT_WIDTH / 2, TFT_HEIGHT/4 * i + TFT_HEIGHT/4 , 20, TFT_BLUE);
	if (pwmValues[0])
	{
		drawing_sprite.fillCircle(67, 120, pwmValues[0] / 11, TFT_BLUE);
		// Serial.printf("Seconds lefts : %lf\n", timerAlarmReadSeconds(timers[0]));
	}
	if (pwmValues[1])
	{
		drawing_sprite.fillCircle(27, 190, pwmValues[1] / 11, TFT_BLUE);
	}
	if (pwmValues[2])
	{
		drawing_sprite.fillCircle(108, 190, pwmValues[2] / 11, TFT_BLUE);
	}
	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();
}

void drawNetworkActivity()
{
	TFT_eSprite drawing_sprite = TFT_eSprite(&tft);

	drawing_sprite.setColorDepth(8);
	drawing_sprite.createSprite(tft.width(), tft.height());
	drawing_sprite.fillSprite(TFT_BLACK);
	drawing_sprite.setTextSize(1);
	drawing_sprite.setTextFont(1);
	drawing_sprite.setTextDatum(MC_DATUM);

	drawing_sprite.setCursor(0, 0);
	uint16_t v = analogRead(ADC_PIN);
	float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Voltage : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%.2fv\n", battery_voltage);
	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.setCursor(0, 13);

	drawing_sprite.printf("AP SSID: ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%s\n", APssid);
	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("AP PSSWD: ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%s\n", APpassword);

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Connected clients : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n", WiFi.softAPgetStationNum());

	wifi_sta_list_t wifi_sta_list;
	tcpip_adapter_sta_list_t adapter_sta_list;

	memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
	memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));

	esp_wifi_ap_get_sta_list(&wifi_sta_list);
	tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);

	for (int i = 0; i < adapter_sta_list.num; i++)
	{

		drawing_sprite.setTextColor(TFT_YELLOW);

		drawing_sprite.setCursor(0, 50 + (i * 14));

		// drawing_sprite.println("");

		tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];

		//drawing_sprite.setTextColor(TFT_BLUE);

		// drawing_sprite.print("MAC: ");
		// drawing_sprite.setTextColor(TFT_WHITE);

		// for(int i = 0; i< 6; i++){

		// 	drawing_sprite.printf("%02X", station.mac[i]);
		// 	if(i<5)drawing_sprite.print(":");
		// }

		drawing_sprite.setTextColor(TFT_BLUE);

		drawing_sprite.print("\nIP:  ");
		drawing_sprite.setTextColor(TFT_WHITE);

		drawing_sprite.println(ip4addr_ntoa(&(station.ip)));
	}

	drawBatteryLevel(&drawing_sprite, 100, 00, battery_voltage);

	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();
}

void set_pwm0(int pwm)
{
	ledcWrite(0, pwm);
}

void set_pwm1(int pwm)
{
	ledcWrite(1, pwm);
}

void set_pwm2(int pwm)
{
	ledcWrite(2, pwm);
}

void stop_pwm0(void)
{
	if (timerPansements[0] <= 0)
	{
		Serial.printf("End of timer 0\n");
		ledcWrite(0, 0);
		timerAlarmDisable(timers[0]);
	}
	timerPansements[0]--;
	// timerAlarmDisable(timers[0]);
}

void stop_pwm1(void)
{
	// ledcWrite(1, 0);
	if (timerPansements[1] <= 0)
	{
		Serial.printf("End of timer 1\n");
		ledcWrite(1, 0);
		timerAlarmDisable(timers[1]);
	}
	timerPansements[1]--;
}

void stop_pwm2(void)
{
	ledcWrite(2, 0);
	if (timerPansements[2] <= 0)
	{
		Serial.printf("End of timer 2\n");
		ledcWrite(2, 0);
		timerAlarmDisable(timers[2]);
	}
	timerPansements[2]--;
}

void look_for_udp_message()
{
	int packetSize = Udp.parsePacket();
	if (packetSize)
	{
		int len = Udp.read(incomingPacket, 255);
		if (len > 0)
		{
			incomingPacket[len] = 0;
		}
		Serial.printf("UDP packet contents: %s\n", incomingPacket);
		convertedPacket = String(incomingPacket);
		Serial.println(convertedPacket);
		Serial.printf("Debug indexof = P:%d, D:%d, I:%d\n", convertedPacket.indexOf('P'), convertedPacket.indexOf('D'), convertedPacket.indexOf("I"));

		if (convertedPacket.indexOf("P") > -1 && convertedPacket.indexOf("D") > -1 && convertedPacket.indexOf("I") > -1)
		{
			//t_data_task dataTask;

			int duration = convertedPacket.substring(convertedPacket.indexOf("D") + 1).toInt();
			int intensity = convertedPacket.substring(convertedPacket.indexOf("I") + 1).toInt();
			int pin = convertedPacket.substring(convertedPacket.indexOf("P") + 1).toInt();

			if (pin <= 3 && pin >= 0)
			{
				timers_end[pin] = esp_timer_get_time() / 1000 + duration;
				g_set_pwm[pin](intensity);
				pwmValues[pin] = intensity;
			}
		}
	}
}

void compassArraw(TFT_eSprite * sprite, int x, int y, float angle)
{
	TFT_eSprite direction = TFT_eSprite(&tft);
	direction.setColorDepth(8);
	direction.createSprite(50, 20);
	direction.fillRect(5, 5, 40, 10, TFT_BLUE);
	direction.fillTriangle(40,0,50,10,40,20,TFT_BLUE);
	direction.setPivot(25, 10);
	// direction.setRotation(60);
	
	
	// direction.pushSprite(50, 50);
	TFT_eSprite directionBack = TFT_eSprite(&tft);
	directionBack.setColorDepth(8);
	directionBack.createSprite(50, 50);

	direction.pushRotated(&directionBack, angle);
	directionBack.pushToSprite((TFT_eSprite*)sprite,(int32_t) x,(int32_t) y);
	(*sprite).drawCircle(x + 25, y + 25, 30, TFT_WHITE);
}

void drawCursors(TFT_eSprite *sprite, int x, int y, int w, int h, int min, int max, int value, uint32_t color)
{
	if (value < (max - min) / 2 + min)
	{
		(*sprite).fillRect(x, y + (h / 2) - map(value, (max - min) / 2 + min, min, 0, h / 2), w, map(value, (max - min) / 2 + min, min, 0, h / 2), color);
	}
	else
	{
		(*sprite).fillRect(x, y + (h / 2), w, map(value, (max - min) / 2 + min, max, 0, h / 2), color);
	}
	(*sprite).drawRect(x, y, w, h, TFT_WHITE);

	// (*sprite).fillRect(x, y, 10, 50, TFT_WHITE);
}

void sendOscMessage(char *oscPrefix, String oscMessage)
{
	OSCMessage msg(oscPrefix);
	// String message = String(accel_event.acceleration.x, 3)
	// 		+ " " + String(accel_event.acceleration.y, 3)
	// 		+ " " + String(accel_event.acceleration.z, 3);
	// String message = "Hello world" ;
	char messageBuffer[50];
	oscMessage.toCharArray(messageBuffer, 50);
	Serial.println(messageBuffer);
    msg.add(messageBuffer);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}

void sendOscFloatMessage(char *oscPrefix, float oscMessage)
{
	OSCMessage msg(oscPrefix);
	// String message = String(accel_event.acceleration.x, 3)
	// 		+ " " + String(accel_event.acceleration.y, 3)
	// 		+ " " + String(accel_event.acceleration.z, 3);
	// String message = "Hello world" ;
	// char messageBuffer[50];
	// oscMessage.toCharArray(messageBuffer, 50);
	Serial.println(oscMessage);
    msg.add(oscMessage);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}


void drawGyroscopActivity(void)
{
	static bool isCalibrated = false;
	static int calMinX = 0;
	static int calMaxX = 0;
	static int calMinY = 0;
	static int calMaxY = 0;
	
	if (oscAddressChanged == true)
	{
		noInterrupts();
		delay(50);
		EEPROM.writeUInt(0, oscAddress);
		EEPROM.commit();
		delay(50);
		interrupts();
		oscAddressChanged = false;
	}






    // for (int i = 0; i < 512; i++) {
    //   EEPROM.write(i, 0);
    // }
    // EEPROM.commit();


	TFT_eSprite drawing_sprite = TFT_eSprite(&tft);
	drawing_sprite.setColorDepth(8);
	drawing_sprite.createSprite(tft.width(), tft.height());

	drawing_sprite.fillSprite(TFT_BLACK);
	drawing_sprite.setTextSize(1);
	drawing_sprite.setTextFont(1);
	drawing_sprite.setTextColor(TFT_GREEN);
	drawing_sprite.setTextDatum(MC_DATUM);
	drawing_sprite.setCursor(0, 0);

	uint16_t v = analogRead(ADC_PIN);
	float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Voltage : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%.2fv\n\n", battery_voltage);
	drawBatteryLevel(&drawing_sprite, 100, 00, battery_voltage);
	drawing_sprite.setTextColor(TFT_WHITE);


	sensors_event_t accel_event;
	sensors_event_t mag_event;
	accel.getEvent(&accel_event);
	mag.getEvent(&mag_event);
	// mag.getSensor(&event);
	// gyro.getEvent(&event);
	gyro.read();


	// Calculate the angle of the vector y,x

	// Normalize to 0-360


	if (isCalibrated == false)
	{
		calMinX = mag_event.magnetic.x;
		calMaxX = mag_event.magnetic.x + 1;
		calMinY = mag_event.magnetic.y;
		calMaxY = mag_event.magnetic.y + 1;
		isCalibrated = true;
	}
	else
	{
		if (mag_event.magnetic.x < calMinX)
			calMinX = mag_event.magnetic.x;
		else if (mag_event.magnetic.x > calMaxX)
			calMaxX = mag_event.magnetic.x;
		if (mag_event.magnetic.y < calMinY)
			calMinY = mag_event.magnetic.y;
		else if (mag_event.magnetic.y > calMaxY)
			calMaxY = mag_event.magnetic.y;
	}


	int mag_xcal = map(mag_event.magnetic.x, calMinX, calMaxX, -1000, 1000);
	int mag_ycal = map(mag_event.magnetic.y, calMinY, calMaxY, -1000, 1000);
	float heading = atan2((double)mag_xcal, (double)mag_ycal);
	// if (heading < 0) {
	// 	heading = 360 + heading;
	// }

	compassArraw(&drawing_sprite, 45, 175, (-(int)(heading * 180 / PI)));

	drawing_sprite.setCursor(2, 15);
	drawing_sprite.printf("osc addr : /%d", oscAddress);


	drawing_sprite.setCursor(7, 30);
	drawing_sprite.printf("Accel");
	drawing_sprite.setCursor(4, 45);
	drawing_sprite.printf("X");
	drawing_sprite.setCursor(19, 45);
	drawing_sprite.printf("Y");
	drawing_sprite.setCursor(34, 45);
	drawing_sprite.printf("Z");

	drawing_sprite.setCursor(56, 30);
	drawing_sprite.printf("Gyro");

	drawing_sprite.setCursor(49, 45);
	drawing_sprite.printf("X");
	drawing_sprite.setCursor(64, 45);
	drawing_sprite.printf("Y");
	drawing_sprite.setCursor(79, 45);
	drawing_sprite.printf("Z");


	drawing_sprite.setCursor(105, 30);
	drawing_sprite.printf("Mag");

	drawing_sprite.setCursor(94, 45);
	drawing_sprite.printf("X");
	drawing_sprite.setCursor(109, 45);
	drawing_sprite.printf("Y");
	drawing_sprite.setCursor(124, 45);
	drawing_sprite.printf("Z");

	drawCursors(&drawing_sprite, 0, 60, 12, 100, -40, 40, accel_event.acceleration.x, TFT_RED);
	drawCursors(&drawing_sprite, 15, 60, 12, 100, -40, 40, accel_event.acceleration.y, TFT_RED);
	drawCursors(&drawing_sprite, 30, 60, 12, 100, -40, 40, accel_event.acceleration.z, TFT_RED);

	drawCursors(&drawing_sprite, 45, 60, 12, 100, -37000, 37000, gyro.g.x, TFT_RED);
	drawCursors(&drawing_sprite, 60, 60, 12, 100, -37000, 37000, gyro.g.y, TFT_RED);
	drawCursors(&drawing_sprite, 75, 60, 12, 100, -37000, 37000, gyro.g.z, TFT_RED);

	drawCursors(&drawing_sprite, 90, 60, 12, 100, -1000, 1000, mag_xcal, TFT_RED);
	drawCursors(&drawing_sprite, 105, 60, 12, 100, -1000, 1000, mag_ycal, TFT_RED);
	drawCursors(&drawing_sprite, 120, 60, 12, 100, -200, 200, mag_event.magnetic.z, TFT_RED);
	// event.acceleration.
	
	float gyroscope = map(sqrtf(gyro.g.x * gyro.g.x \
		 + gyro.g.y * gyro.g.y \
		 + gyro.g.z * gyro.g.z), 0, 37000, 0, 50);
	float acceleration = map(sqrtf(accel_event.acceleration.x * accel_event.acceleration.x \
		 + accel_event.acceleration.y * accel_event.acceleration.y \
		 + accel_event.acceleration.z * accel_event.acceleration.z), 0, 40, 0, 50);
	
	drawing_sprite.setCursor(5, 170);
	drawing_sprite.printf("Accel");
	drawing_sprite.fillRect(10, 240 - acceleration, 20, acceleration, TFT_RED);
	drawing_sprite.drawRect(10, 190, 20, 50, TFT_WHITE);
	
	drawing_sprite.setCursor(108, 170);
	drawing_sprite.printf("Gyro");
	drawing_sprite.fillRect(110, 240 - gyroscope, 20, gyroscope, TFT_RED);
	drawing_sprite.drawRect(110, 190, 20, 50, TFT_WHITE);

	
	Serial.print("G ");
	Serial.print("X: ");
	Serial.print((int)mag_event.magnetic.x);
	Serial.print(" Y: ");
	Serial.print((int)mag_event.magnetic.y);
	Serial.print(" Z: ");
	Serial.println((int)mag_event.magnetic.z);

	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();


	float accel_x = fmap(accel_event.acceleration.x, -40, 40, -100, 100);
	float accel_y = fmap(accel_event.acceleration.y, -40, 40, -100, 100);
	float accel_z = fmap(accel_event.acceleration.z, -40, 40, -100, 100);
	float gyro_x = fmap(gyro.g.x, -37000, 37000, -100, 100);
	float gyro_y = fmap(gyro.g.y, -37000, 37000, -100, 100);
	float gyro_z = fmap(gyro.g.z, -37000, 37000, -100, 100);
	float magnet_x = fmap(mag_event.magnetic.x, -100, 100, -100, 100);
	float magnet_y = fmap(mag_event.magnetic.y, -100, 100, -100, 100);
	float magnet_z = fmap(mag_event.magnetic.z, -100, 100, -100, 100);

	float gyro_normal = map(sqrtf(gyro.g.x * gyro.g.x \
		 + gyro.g.y * gyro.g.y \
		 + gyro.g.z * gyro.g.z), 0, 37000, 0, 100);
	float accel_normal = map(sqrtf(accel_event.acceleration.x * accel_event.acceleration.x \
		 + accel_event.acceleration.y * accel_event.acceleration.y \
		 + accel_event.acceleration.z * accel_event.acceleration.z), 0, 40, 0, 100);



	sendOscFloatMessage("/accelerometer_x", (float)accel_x);
	sendOscFloatMessage("/accelerometer_y", (float)accel_y);
	sendOscFloatMessage("/accelerometer_z", (float)accel_z);
	sendOscFloatMessage("/accelerometer_normal", (float)accel_normal);

	sendOscFloatMessage("/gyroscope_x", (float)gyro_x);
	sendOscFloatMessage("/gyroscope_y", (float)gyro_y);
	sendOscFloatMessage("/gyroscope_z", (float)gyro_z);
	sendOscFloatMessage("/gyroscope_normal", (float)gyro_normal);

	sendOscFloatMessage("/magnetometer_x", (float)magnet_x);
	sendOscFloatMessage("/magnetometer_y", (float)magnet_y);
	sendOscFloatMessage("/magnetometer_z", (float)magnet_z);


	// sendOscMessage("/accelerometer",  String(accel_x)
	// 		+ " " + String(accel_y)
	// 		+ " " + String(accel_z));
	// sendOscMessage("/gyroscope",  String(gyro_x)
	// 		+ " " + String(gyro_y)
	// 		+ " " + String(gyro_z));
	// sendOscMessage("/magnetometer",  String(magnet_x)
	// 		+ " " + String(magnet_y)
	// 		+ " " + String(magnet_z));



}

void loop()
{
	if (current_mode == STA_MODE)
	{
		look_for_udp_message();
		for (int i = 0; i < 3; i++)
		{
			if (timers_end[i] != 0 && timers_end[i] < esp_timer_get_time() / 1000)
			{
				g_set_pwm[i](0);
				timers_end[i] = 0;
				pwmValues[i] = 0;
			}
		}
		drawMotorsActivity();
	}
	else if (current_mode == AP_MODE)
	{
		drawNetworkActivity();
	}
	else if (current_mode == GYRO_MODE)
	{
		drawGyroscopActivity();
	}
	//Serial.println(".");
	delay(25);
	// delay(500);
	// put your main code here, to run repeatedly:
}