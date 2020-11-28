#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"
#include <WiFiUdp.h>
#include "esp_wifi.h"


#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL              4   // Display backlight control pin
#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
#define BUTTON_1            35
#define BUTTON_2            0

#define MOTOR_1				25
#define MOTOR_2				26
#define MOTOR_3				27

const int motorFreq = 5000;
const int motorResolution = 8;
const int motorChannel1 = 0;
const int motorChannel2 = 1;
const int motorChannel3 = 2;

typedef	struct 	s_data_task
{
	int duration;
	int pwm;
	int motor_id;
	TaskHandle_t thisTaskHandler;
}				t_data_task;


// typedef void(*t_task_func)(void *param);

enum e_wifi_modes {
	NONE_MODE = 0,
	STA_MODE,
	AP_MODE
};



t_data_task g_data_task[3];

int vref = 1100;
int timers_end[3] = {0,0,0};

#define BLACK 0x0000
#define WHITE 0xFFFF
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

e_wifi_modes current_wifi_mode;
// const char* ssid = "Freebox-0E3EAE";
// const char* password =  "taigaest1chien";

const char* ssid = "tripodesAP";
const char* password =  "44448888";





const char *APssid = "tripodesAP";
const char *APpassword = "44448888";


WiFiUDP Udp;
unsigned int localUdpPort = 49141;
char incomingPacket[255];
String convertedPacket;
char  replyPacket[] = "Message received";


void set_pwm0(int pwm);
void set_pwm1(int pwm);
void set_pwm2(int pwm);

typedef void(*t_set_pwm)(int pwm);

#define TASK_NUMBER 3
static const t_set_pwm	g_set_pwm[TASK_NUMBER] = {
	(t_set_pwm)set_pwm0,
	(t_set_pwm)set_pwm1,
	(t_set_pwm)set_pwm2,
};

void stop_pwm0(void);
void stop_pwm1(void);
void stop_pwm2(void);

typedef void(*t_stop_pwm)(void);

#define TASK_NUMBER 3
static const t_stop_pwm	g_stop_pwm[TASK_NUMBER] = {
	(t_stop_pwm)stop_pwm0,
	(t_stop_pwm)stop_pwm1,
	(t_stop_pwm)stop_pwm2,
};

WiFiClass WiFiAP;

bool	timersActives[3];
int		pwmValues[3];
int		timerPansements[3];
hw_timer_t * timers[4] = {NULL, NULL, NULL, NULL};



const char* wl_status_to_string(int ah) {
	switch (ah) {
		case WL_NO_SHIELD: return "WL_NO_SHIELD";
		case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
		case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
		case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
		case WL_CONNECTED: return "WL_CONNECTED";
		case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
		case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
		case WL_DISCONNECTED: return "WL_DISCONNECTED";
		default: return "ERROR NOT VALID WL";
	}
}

const char* eTaskGetState_to_string(int ah) {
	switch (ah) {
		case eRunning: return "eRunning";
		case eReady: return "eReady";
		case eBlocked: return "eBlocked";
		case eSuspended: return "eSuspended";
		case eDeleted: return "eDeleted";
		default: return "ERROR NOT STATE";
	}
}




void button_init()
{
		btn1.setPressedHandler([](Button2 & b) {
				Serial.println("Bouton A pressed");
		if (current_wifi_mode == NONE_MODE)
		{
			current_wifi_mode = STA_MODE;
		}
	});

		btn2.setPressedHandler([](Button2 & b) {
				Serial.println("Bouton B pressed");
		if (current_wifi_mode == NONE_MODE)
		{
			current_wifi_mode = AP_MODE; 
		}
	});
}

void button_loop()
{
		btn1.loop();
		btn2.loop();
}

void  call_buttons(void)
{
	button_loop();
}


void showVoltage()
{
	static uint64_t timeStamp = 0;
	if (millis() - timeStamp > 1000) {
		timeStamp = millis();
		uint16_t v = analogRead(ADC_PIN);
		float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
		String voltage = "Voltage :" + String(battery_voltage) + "V";
		Serial.println(voltage);
		tft.fillScreen(TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString(voltage,  tft.width() / 2, tft.height() / 2 );
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
		if(millis() - timeStamp > 10000)
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
	WiFi.softAP(APssid, APpassword);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	tft.printf("AP addr: %s\n", myIP.toString().c_str());
}



void setup() {
	// put your setup code here, to run once:
	current_wifi_mode = NONE_MODE;
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	// WiFiAP.begin(APssid, APpassword);



				
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



	if (TFT_BL > 0) {                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
			pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
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


	for(;;)
	{
		Serial.print("tour de boucle :");
		Serial.println(current_wifi_mode);
		if (current_wifi_mode == STA_MODE)
		{
			sta_setup();
			break;
		}
		else if (current_wifi_mode == AP_MODE)
		{
			ap_setup();
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
	drawing_sprite.printf("%llds\n", esp_timer_get_time()/1000000);
drawBatteryLevel(&drawing_sprite, 100, 00, battery_voltage);
	

	//uint32_t color1 = TFT_BLUE;
	uint32_t color2 = TFT_WHITE;
	drawing_sprite.drawCircle(67, 120 , 26, color2);
	drawing_sprite.drawCircle(27, 190 , 26, color2);
	drawing_sprite.drawCircle(108, 190 , 26, color2);
	drawing_sprite.drawLine(15, 167, 40, 150, color2);
	drawing_sprite.drawLine(40, 150, 40, 120, color2);
	drawing_sprite.drawLine(93, 120, 93, 150, color2);
	drawing_sprite.drawLine(93, 150, 120, 167, color2);
	drawing_sprite.drawLine(100, 215, 67, 195, color2);
	drawing_sprite.drawLine(67, 195, 35, 215, color2);

			//drawing_sprite.drawCircle(TFT_WIDTH / 2, TFT_HEIGHT/4 * i + TFT_HEIGHT/4 , 20, TFT_BLUE);
	if (pwmValues[0])
	{
		drawing_sprite.fillCircle(67, 120 ,  pwmValues[0] / 11, TFT_BLUE);
		// Serial.printf("Seconds lefts : %lf\n", timerAlarmReadSeconds(timers[0]));
	}
	if (pwmValues[1])
	{
		drawing_sprite.fillCircle(27, 190 ,  pwmValues[1] / 11, TFT_BLUE);
	}
	if (pwmValues[2])
	{
		drawing_sprite.fillCircle(108, 190 ,  pwmValues[2] / 11, TFT_BLUE);
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
drawBatteryLevel(&drawing_sprite, 100, 00, battery_voltage);

	

	
		wifi_sta_list_t wifi_sta_list;
	tcpip_adapter_sta_list_t adapter_sta_list;
 
	memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
	memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
 
	esp_wifi_ap_get_sta_list(&wifi_sta_list);
	tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
 
	for (int i = 0; i < adapter_sta_list.num; i++) {

		drawing_sprite.setTextColor(TFT_YELLOW);

		drawing_sprite.println("");
 
		tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
 
		drawing_sprite.setTextColor(TFT_BLUE);

		drawing_sprite.print("MAC: ");
		drawing_sprite.setTextColor(TFT_WHITE);
 
		for(int i = 0; i< 6; i++){
			
			drawing_sprite.printf("%02X", station.mac[i]);  
			if(i<5)drawing_sprite.print(":");
		}
		drawing_sprite.setTextColor(TFT_BLUE);
 
		drawing_sprite.print("\nIP:  ");  
		drawing_sprite.setTextColor(TFT_WHITE);

		drawing_sprite.println(ip4addr_ntoa(&(station.ip)));    
	}
 




	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();
	
}



void	set_pwm0(int pwm)
{
	ledcWrite(0, pwm);
}

void	set_pwm1(int pwm)
{
	ledcWrite(1, pwm);
}

void	set_pwm2(int pwm)
{
	ledcWrite(2, pwm);
}

void	stop_pwm0(void)
{
	if (timerPansements[0] <= 0)
	{
		Serial.printf("End of timer 0\n");
		ledcWrite(0, 0);
		timerAlarmDisable(timers[0]);	}
	timerPansements[0]--;
	// timerAlarmDisable(timers[0]);
}

void	stop_pwm1(void)
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

void	stop_pwm2(void)
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
		Serial.printf("Debug indexof = P:%d, D:%d, I:%d\n",convertedPacket.indexOf('P'), convertedPacket.indexOf('D'), convertedPacket.indexOf("I"));
		
		if (convertedPacket.indexOf("P") > -1 && convertedPacket.indexOf("D") > -1 && convertedPacket.indexOf("I") > -1 )
		{
			//t_data_task dataTask;

			int duration = convertedPacket.substring(convertedPacket.indexOf("D") + 1).toInt();
			int intensity = convertedPacket.substring(convertedPacket.indexOf("I") + 1).toInt();
			int pin = convertedPacket.substring(convertedPacket.indexOf("P") + 1).toInt();
			

			if (pin <= 3 && pin >= 0)
			{
				timers_end[pin] = esp_timer_get_time()/1000 + duration;
				g_set_pwm[pin](intensity);
				pwmValues[pin] = intensity;
			}






		}
		
		
	}
}


void loop() {
	if (current_wifi_mode == STA_MODE)
	{
		look_for_udp_message();
		for (int i = 0; i < 3; i++)
			{
				if (timers_end[i] != 0 && timers_end[i] < esp_timer_get_time()/1000)
				{
					g_set_pwm[i](0);
					timers_end[i] = 0;
					pwmValues[i] = 0;
				}
			}
		drawMotorsActivity();
	}
	else if (current_wifi_mode == AP_MODE)
	{
		drawNetworkActivity();
	}
	//Serial.println(".");
	delay(25);
	// put your main code here, to run repeatedly:
}