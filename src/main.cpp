
#include "ESPAsyncWebServer.h"
#include "tripodes.h"
#include <string>
#include <HTTPClient.h>
#include <list>
// #include <ESP32Ping.h>

// #include <Adafruit_L3GD20_U.h>
// tripod
// perf8888
// const char *ssid = "tripodesAP";
// const char *password = "44448888";
// const char *ssid = "tripod";
// const char *password = "perf8888";
// const char *ssid = "Thinking Sound 2.4g";
// const char *password = "rock&roll";

// const char *ssid = "Dourr";
// const char *password = "Akiraestlepluscooldeschien28os";

TaskHandle_t Task1;

TaskHandle_t AudioTask;

std::mutex sta_list_mutex;

char *ssid;
char *password;
char *APssid;
char *APpassword;
// char *audioHost;
char *tripode_id;
int16_t audioVolume = 21;
int16_t fractal_state_pos_x = 0;
int16_t fractal_state_pos_y = 10;
int16_t glyph_pos_x = 48;
int16_t glyph_pos_y = 6;

// Audio audio;

L3G gyro;

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_LSM303DLH_Mag_Unified mag = Adafruit_LSM303DLH_Mag_Unified(12345);
// Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
const int motorFreq = 50;
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
	NONE_MODE    = 0,
	AP_MASK      = 0b00001,
	STA_MASK     = 0b00010,

	STD_MODE     = 0b00100,
	SENSORS_MODE = 0b01000,
	DFA_MODE     = 0b01100,
	AP_MODE      = 0b10000,
	RUNE_MODE    = 0b10100,
	MIDI_MODE    = 0b11000,
	MODE_MASK    = 0b11100,
};

t_data_task g_data_task[3];

double notefrequencies[] = {
	16.35,//    0 C0
	17.32,//    1 c0
	18.35,//    2 D0
	19.45,//    3 d0
	20.60,//    4 E0
	21.83,//    5 F0
	23.12,//    6 f0
	24.50,//    7 G0
	25.96,//    8 g0
	27.50,//    9 A0
	29.14,//   10 a0
	30.87,//   11 B0
	32.70,//   12 C1
	34.65,//   13 c1
	36.71,//   14 D1
	38.89,//   15 d1
	41.20,//   16 E1
	43.65,//   17 F1
	46.25,//   18 f1
	49.00,//   19 G1
	51.91,//   20 g1
	55.00,//   21 A1
	58.27,//   22 a1
	61.74,//   23 B1
	65.41,//   24 C2
	69.30,//   25 c2
	73.42,//   26 D2
	77.78,//   27 d2
	82.41,//   28 E2
	87.31,//   29 F2
	92.50,//   30 f2
	98.00,//   31 G2
	103.83,//  32 g2
	110.00,//  33 A2
	116.54,//  34 a2
	123.47,//  35 B2
	130.81,//  36 C3
	138.59,//  37 c3
	146.83,//  38 D3
	155.56,//  39 d3
	164.81,//  40 E3
	174.61,//  41 F3
	185.00,//  42 f3
	196.00,//  43 G3
	207.65,//  44 g3
	220.00,//  45 A3
	233.08,//  46 a3
	246.94,//  47 B3
	261.63,//  48 C4
	277.18,//  49 c4
	293.66,//  50 D4
	311.13,//  51 d4
	329.63,//  52 E4
	349.23,//  53 F4
	369.99,//  54 f4
	293.00,//  55 G4
	415.30,//  56 g4
	440.00,//  57 A4
	466.16,//  58 a4
	493.88,//  59 B4
	523.25,//  60 C5
	554.37,//  61 c5
	587.33,//  62 D5
	622.25,//  63 d5
	659.25,//  64 E5
	698.46,//  65 F5
	739.99,//  66 f5
	783.99,//  67 G5
	830.61,//  68 g5
	880.00,//  69 A5
	932.33,//  70 a5
	987.77,//  71 B5
	
	1046.50,// 72 C6
	1108.73,// 73 c6
	1174.66,// 74 D6
	1244.51,// 75 d6
	1318.51,// 76 E6
	1396.91,// 77 F6
	1479.98,// 78 f6
	1567.98,// 79 G6
	1661.22,// 80 g6
	1760.00,// 81 A6
	1864.66,// 82 a6
	1975.53,// 83 B6

	2093.00,// 84 C7
	2217.46,// 85 c7
	2349.32,// 86 D7
	2489.02,// 87 d7
	2637.02,// 88 E7
	2793.83,// 89 F7
	2959.96,// 90 f7
	3135.96,// 91 G7
	3322.44,// 92 g7
	3520.00,// 93 A7
	3729.31,// 94 a7
	3951.07,// 95 B7

	4186.01,// 96 C8
	4434.91,// 97 c8
	4698.63,// 98 D8
	4978.03,// 99 d8
	5274.04,//100 E8
	5587.65,//101 F8
	5919.91,//101 f8
	6271.93,//102 G8
	6644.88,//103 g8
	7040.00,//104 A8
	7458.62,//105 a8
	7902.13,//106 B8









//Finish : https://pages.mtu.edu/~suits/notefreqs.html










} 


int timers_end[3] = {0, 0, 0};

#define BLACK 0x0000
#define WHITE 0xFFFF
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

// e_wifi_modes current_mode;
uint8_t current_mode = 0;

bool udp_sending = false;
bool osc_sending = false;
bool i2s_on = false;

// const char* ssid = "Freebox-0E3EAE";
// const char* password =  "taigaest1chien";

WiFiUDP Udp;

IPAddress orca_ip(10, 87, 210, 255); // Raspi4 local addr
unsigned int orca_port = 49160;		 // Orca input port

IPAddress usine_ip(10, 0, 1, 14);
unsigned int usine_port = 2002; // remote port to receive OSC

const IPAddress outIp(192, 168, 0, 12); // remote IP of your computer
// const IPAddress outIp(192,168,0,41);        // remote IP of your computer

// const IPAddress outIp(192,168,56,1);        // remote IP of your computer
const unsigned int outPort = 2002; // remote port to receive OSC

AsyncWebServer server(80);

bool oscAddressChanged = false;
unsigned int oscAddress = 1;
unsigned int outUdpPort = 49100;
unsigned int localUdpPort = 49141;
char incomingPacket[255];
String convertedPacket;
char replyPacket[] = "Message received";

void set_pwm0(int pwm, int tonality);
void set_pwm1(int pwm, int tonality);
void set_pwm2(int pwm, int tonality);

typedef void (*t_set_pwm)(int pwm, int tonality);

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

std::list<s_sta_list> sta_list;

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

double fmap(double x, double in_min, double in_max, double out_min, double out_max)
{
	const double dividend = out_max - out_min;
	const double divisor = in_max - in_min;
	const double delta = x - in_min;
	if (divisor == 0)
	{
		log_e("Invalid map input range, min == max");
		return -1; // AVR returns -1, SAM returns 0
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
			current_mode = STD_MODE | STA_MASK;
		}
		else if (current_mode & STA_MASK)
		{
			if ((current_mode & MODE_MASK) == STD_MODE)
			{
				current_mode = SENSORS_MODE | STA_MASK;
			}
			else if ((current_mode & MODE_MASK) == SENSORS_MODE)
			{
				current_mode = DFA_MODE | STA_MASK;
			}
			else if ((current_mode & MODE_MASK) == DFA_MODE)
			{
				current_mode = RUNE_MODE | STA_MASK;
			}
			else if ((current_mode & MODE_MASK) == RUNE_MODE)
			{
				current_mode = MIDI_MODE | STA_MASK;
			}
			else if ((current_mode & MODE_MASK) == MIDI_MODE)
			{
				current_mode = STD_MODE | STA_MASK;
			}
		}
		else if (current_mode & AP_MASK)
		{
			if ((current_mode & MODE_MASK) == STD_MODE)
			{
				Serial.printf("----- STD mode : %d\n", current_mode);
				current_mode = SENSORS_MODE | AP_MASK;
				Serial.printf("--AF- STD mode : %d\n", current_mode);
			}
			else if ((current_mode & MODE_MASK) == SENSORS_MODE)
			{
				Serial.printf("----- SENSOR mode : %d\n", current_mode);
				current_mode = DFA_MODE | AP_MASK;
				Serial.printf("--AF- SENSOR mode : %d\n", current_mode);
			}
			else if ((current_mode & MODE_MASK) == DFA_MODE)
			{
				Serial.printf("----- DFA mode : %d\n", current_mode);
				current_mode = RUNE_MODE | AP_MASK;
				Serial.printf("--AF- DFA mode : %d\n", current_mode);
			}
			else if ((current_mode & MODE_MASK) == RUNE_MODE)
			{
				Serial.printf("----- RUNE mode : %d\n", current_mode);
				current_mode = MIDI_MODE | AP_MASK;
				Serial.printf("--AF- RUNE mode : %d\n", current_mode);
			}
			else if ((current_mode & MODE_MASK) == MIDI_MODE)
			{
				Serial.printf("----- MIDI mode : %d\n", current_mode);
				current_mode = AP_MODE | AP_MASK;
				Serial.printf("--AF- MIDI mode : %d\n", current_mode);
			}
			else if ((current_mode & MODE_MASK) == AP_MODE)
			{
				Serial.printf("----- Ap mode : %d\n", current_mode);
				current_mode = STD_MODE | AP_MASK;
				Serial.printf("--AF- Ap mode : %d\n", current_mode);
			}
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
			// current_mode = AP_MODE;
			current_mode = AP_MODE | AP_MASK;
		}
		else if (current_mode)
		{
			udp_sending = !udp_sending;
		}
	}
	if (click_type == DOUBLE_CLICK)
	{
		osc_sending = !osc_sending;
	}
	if (click_type == TRIPLE_CLICK)
	{
		i2s_on = !i2s_on;
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
		float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);
		String voltage = "Voltage :" + String(battery_voltage) + "V";
		Serial.println(voltage);
		tft.fillScreen(TFT_BLACK);
		tft.setTextDatum(MC_DATUM);
		tft.drawString(voltage, tft.width() / 2, tft.height() / 2);
	}
}

// Need transform to not trigger the watchdog
String get_sta_list()
{
	// <ul>
	//	<li>Ip1</li>
	//	<li>Ip2</li>
	//	....
	// </ul>
	sta_list_mutex.lock();
	String html_code = "<ul>\n";
	for (const t_sta_list &elem : sta_list)
	{
		Serial.print("Ip : ");
		Serial.print(elem.ip_adress);
		Serial.print(" has website : ");
		Serial.println(elem.has_website);
		if (elem.has_website)
		{
			html_code += "	<li><a href=\"http://" + elem.ip_adress + "\">" + elem.ip_adress + "</a> (Tripode) </li>\n";
		}
		else
		{
			html_code += "	<li>" + elem.ip_adress + "</li>\n";
		}
	}

	// HTTPClient http_request;
	// http_request.begin("http://" + String(ip_char) );

	// String httpcode =  String(http_request.GET());
	// http_request.end();

	html_code += "<ul>";
	sta_list_mutex.unlock();
	return (html_code);
}

String processor(const String &var)
{
	Serial.println(var);
	if (var == "USINEIP")
	{
		return (usine_ip.toString());
	}
	else if (var == "USINEPORT")
	{
		char *intStr;
		intStr = (char *)malloc(sizeof(char) * 15);
		intStr = itoa(usine_port, intStr, 10);
		String StringPort = String(intStr);
		free(intStr);
		return (StringPort);
	}
	else if (var == "ORCAIP")
	{
		return (orca_ip.toString());
	}
	else if (var == "ORCAPORT")
	{
		char *intStr;
		intStr = (char *)malloc(sizeof(char) * 15);
		intStr = itoa(orca_port, intStr, 10);
		String StringPort = String(intStr);
		free(intStr);
		return (StringPort);
	}
	else if (var == "TRIPODEID")
	{
		String string_tripode_id = String(tripode_id);
		return (string_tripode_id);
	}
	else if (var == "FRACTALSTATEPOSX")
	{
		char *intStr;
		intStr = (char *)malloc(sizeof(char) * 15);
		intStr = itoa(fractal_state_pos_x, intStr, 10);
		String StringPos = String(intStr);
		free(intStr);
		return (StringPos);
	}
	else if (var == "FRACTALSTATEPOSY")
	{
		char *intStr;
		intStr = (char *)malloc(sizeof(char) * 15);
		intStr = itoa(fractal_state_pos_y, intStr, 10);
		String StringPos = String(intStr);
		free(intStr);
		return (StringPos);
	}
	else if (var == "GLYPHPOSX")
	{
		char *intStr;
		intStr = (char *)malloc(sizeof(char) * 15);
		intStr = itoa(glyph_pos_x, intStr, 10);
		String StringPos = String(intStr);
		free(intStr);
		return (StringPos);
	}
	else if (var == "GLYPHPOSY")
	{
		char *intStr;
		intStr = (char *)malloc(sizeof(char) * 15);
		intStr = itoa(glyph_pos_y, intStr, 10);
		String StringPos = String(intStr);
		free(intStr);
		return (StringPos);
	}
	else if (var == "STASSID")
	{
		String string_ssid = String(ssid);
		return (string_ssid);
	}
	else if (var == "STAPASSWORD")
	{
		String string_password = String(password);
		return (string_password);
	}
	else if (var == "APSSID")
	{
		String string_ssid = String(APssid);
		return (string_ssid);
	}
	else if (var == "APPASSWORD")
	{
		String string_password = String(APpassword);
		return (string_password);
	}
	else if (var == "STALIST")
	{
		return (get_sta_list());
	}
	// else if (var == "AUDIOHOST")
	// {
	// 	String string_audioHost = String(audioHost);
	// 	return (string_audioHost);
	// }
	// else if (var == "AUDIOVOLUME")
	// {
	// 	char *intStr;
	// 	intStr = (char *)malloc(sizeof(char) * 15);
	// 	intStr = itoa(audioVolume, intStr, 10);
	// 	String StringVol = String(intStr);
	// 	free(intStr);
	// 	return (StringVol);
	// }
	return String();
}

uint8_t get_octet(char *str, uint8_t n)
{
	size_t i = 0;
	uint8_t n_number = 0;
	uint8_t last_octet = 0;
	while (str[i])
	{
		if (i == 0 || str[i - 1] == '.')
		{
			last_octet = atoi(&str[i]);
		}
		if (str[i] == '.')
		{
			n_number++;
		}
		i++;
		if (n_number == n)
		{
			return (last_octet);
		}
	}
	return (last_octet);
}

bool is_key_matching(char *key, char *file, size_t index)
{
	size_t i = 0;
	while (file[index] == key[i] && file[index] != '\n' && file[index] != '\r' && file[index] && key[i])
	{
		index++;
		i++;
	}
	if (key[i] == '\0')
	{
		return (true);
	}
	return (false);
}

void update_sta_list(void *params)
{
	wifi_sta_list_t wifi_sta_list;
	tcpip_adapter_sta_list_t adapter_sta_list;

	for (;;)
	{

		memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
		memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));

		esp_wifi_ap_get_sta_list(&wifi_sta_list);
		tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);

		// sta_list.clear();
		std::list<t_sta_list> tmp_list;

		for (int i = 0; i < adapter_sta_list.num; i++)
		{
			tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
			char *ip_char = ip4addr_ntoa(&(station.ip)); // leak?

			HTTPClient http_request;
			http_request.setConnectTimeout(5000);
			http_request.setTimeout(5000);
			String ip_str = String(ip_char);
			http_request.begin(("http://" + ip_str + "/?test=42").c_str());
			int http_code = http_request.GET();
			// Serial.printf("Req code : %d\n", http_code);
			// String httpcode =  String(http_request.GET());
			bool has_website = http_code == 200 ? true : false;
			// Serial.print("Ip sta :");
			// Serial.println(ip_str);
			tmp_list.push_front((t_sta_list){.ip_adress = ip_str, .has_website = has_website});
			http_request.end();
		}
		sta_list_mutex.lock();
		sta_list = tmp_list;
		sta_list_mutex.unlock();
		// Serial.println("Before");
		delay(1000);
		// Serial.println("After");
	}
}

char *get_value_from_csv(char *file, size_t index)
{
	size_t value_size = 0;
	char *value = 0;

	while (file[index + value_size] != '\0' && file[index + value_size] != '\n' && file[index + value_size] != '\r')
	{
		value_size++;
	}
	value = (char *)malloc(sizeof(char) * (value_size + 1));
	value_size = 0;
	while (file[index + value_size] != '\0' && file[index + value_size] != '\n' && file[index + value_size] != '\r')
	{
		value[value_size] = file[index + value_size];
		value_size++;
	}
	value[value_size] = '\0';
	return (value);
}

char *concatenate_csv(char *buffer, size_t beg_index, size_t end_index, char *value)
{
	size_t new_size = strlen(buffer) + strlen(value) - (end_index - beg_index) + 1;
	char *new_buffer = (char *)malloc(sizeof(char) * new_size);
	size_t old_index = 0;
	size_t new_index = 0;

	while (old_index < beg_index)
	{
		new_buffer[new_index] = buffer[old_index];
		old_index++;
		new_index++;
	}

	for (size_t i = 0; value[i] != '\0'; i++)
	{
		new_buffer[new_index] = value[i];
		new_index++;
	}
	old_index = end_index;
	while (buffer[old_index] != '\0')
	{
		new_buffer[new_index] = buffer[old_index];
		old_index++;
		new_index++;
	}
	new_buffer[new_index] = '\0';
	return (new_buffer);
}

void set_data_to_csv(char *key, char *value)
{
	fs::File file = SPIFFS.open("/data.csv", "r");
	if (!file)
	{
		Serial.println("Failed to open file for reading");
		return;
	}
	Serial.printf("Key : %s\n", key);
	Serial.printf("Value : %s\n", value);
	Serial.printf("File Size : %d\n", file.size());
	Serial.printf("Free Heap : %d\n", ESP.getFreeHeap());
	uint8_t *buff;
	size_t read_index = 1;
	size_t buff_size = file.size();
	buff = (uint8_t *)malloc(sizeof(char) * (buff_size + 1));
	if ((read_index = file.read(buff, (buff_size))) > 0)
	{
		buff[read_index] = '\0';
		Serial.printf("%s", buff);
	}
	else
	{
		Serial.printf("Error reading file\n");
		Serial.printf("Buff : %s\n", buff);
	}

	bool get_value = false;
	for (size_t i = 0; i < file.size(); i++)
	{
		if (i == 0 || buff[i - 1] == '\n' || buff[i - 1] == '\r')
		{
			if (is_key_matching(key, (char *)buff, i))
			{
				get_value = true;
			}
		}
		if (buff[i - 1] == ',' && get_value)
		{
			size_t beg_index = i;
			size_t end_index = i;
			while (buff[end_index] != '\0' && buff[end_index] != '\n' && buff[end_index] != '\r')
			{
				end_index++;
			}
			char *new_buff = concatenate_csv((char *)buff, beg_index, end_index, value);
			// file.print(new_buff);
			Serial.print("Beg");
			Serial.print(new_buff);
			Serial.print("End");
			file.close();
			file = SPIFFS.open("/data.csv", "w");
			file.print(new_buff);
			file.close();
			free(buff);
			free(new_buff);
			return;
		}
	}
	free(buff);

	file.close();
}

char *get_data_from_csv(char *key)
{
	fs::File file = SPIFFS.open("/data.csv");
	if (!file)
	{
		Serial.println("Failed to open file for reading");
		return (0);
	}
	Serial.print("File size: ");
	Serial.println(file.size());
	if (file.size() == 0)
	{
		return (0);
	}
	uint8_t *buff;
	size_t read_index = 1;
	buff = (uint8_t *)malloc(sizeof(char) * (file.size() + 1));
	if ((read_index = file.read(buff, (file.size() + 1))) > 0)
	{
		buff[read_index] = '\0';
		Serial.printf("%s", buff);
	}

	bool get_value = false;
	for (size_t i = 0; i < file.size(); i++)
	{
		if (i == 0 || buff[i - 1] == '\n')
		{
			if (is_key_matching(key, (char *)buff, i))
			{
				get_value = true;
			}
		}
		if (buff[i - 1] == ',' && get_value)
		{
			char *value = get_value_from_csv((char *)buff, i);
			free(buff);
			file.close();
			return (value);
		}
	}

	free(buff);
	file.close();
	return (0);

	// get value from key here
}

// create function to store value from key here

void setup_server_for_ap()
{
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (request->hasParam("orca_ip"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("orca_ip")->value().toCharArray((char *)buff, 50);
					  orca_ip = IPAddress(get_octet((char *)buff, 1), get_octet((char *)buff, 2), get_octet((char *)buff, 3), get_octet((char *)buff, 4));
					  Serial.println(orca_ip.toString());
					  set_data_to_csv("orca_ip", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("orca_port"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("orca_port")->value().toCharArray((char *)buff, 50);
					  orca_port = atoi((char *)buff);
					  set_data_to_csv("orca_port", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("usine_ip"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("usine_ip")->value().toCharArray((char *)buff, 50);
					  usine_ip = IPAddress(get_octet((char *)buff, 1), get_octet((char *)buff, 2), get_octet((char *)buff, 3), get_octet((char *)buff, 4));
					  Serial.println(usine_ip.toString());
					  set_data_to_csv("usine_ip", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("usine_port"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("usine_port")->value().toCharArray((char *)buff, 50);
					  usine_port = atoi((char *)buff);
					  set_data_to_csv("usine_port", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("tripode_id"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("tripode_id")->value().toCharArray((char *)buff, 50);
					  if (tripode_id)
					  {
						  free(tripode_id);
					  }
					  tripode_id = strdup((char *)buff);
					  set_data_to_csv("tripode_id", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("fractal_state_pos_x"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("fractal_state_pos_x")->value().toCharArray((char *)buff, 50);
					  fractal_state_pos_x = atoi((char *)buff);
					  set_data_to_csv("fractal_state_pos_x", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("fractal_state_pos_y"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("fractal_state_pos_y")->value().toCharArray((char *)buff, 50);
					  fractal_state_pos_y = atoi((char *)buff);
					  set_data_to_csv("fractal_state_pos_y", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("glyph_pos_x"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("glyph_pos_x")->value().toCharArray((char *)buff, 50);
					  glyph_pos_x = atoi((char *)buff);
					  set_data_to_csv("glyph_pos_x", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("glyph_pos_y"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("glyph_pos_y")->value().toCharArray((char *)buff, 50);
					  glyph_pos_y = atoi((char *)buff);
					  set_data_to_csv("glyph_pos_y", (char *)buff);
					  free(buff);
				  }

				  if (request->hasParam("sta_ssid"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("sta_ssid")->value().toCharArray((char *)buff, 50);
					  if (ssid)
					  {
						  free(ssid);
					  }
					  ssid = strdup((char *)buff);
					  set_data_to_csv("sta_ssid", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("sta_password"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("sta_password")->value().toCharArray((char *)buff, 50);
					  if (password)
					  {
						  free(password);
					  }
					  password = strdup((char *)buff);
					  set_data_to_csv("sta_password", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("ap_ssid"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("ap_ssid")->value().toCharArray((char *)buff, 50);
					  if (APssid)
					  {
						  free(APssid);
					  }
					  APssid = strdup((char *)buff);
					  set_data_to_csv("ap_ssid", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("ap_password"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("ap_password")->value().toCharArray((char *)buff, 50);
					  if (APpassword)
					  {
						  free(APpassword);
					  }
					  APpassword = strdup((char *)buff);
					  set_data_to_csv("ap_password", (char *)buff);
					  free(buff);
				  }
				//   if (request->hasParam("audio_host"))
				//   {
				// 	  					  uint8_t *buff;
				// 	  buff = (uint8_t *)malloc(sizeof(uint8_t) * 250);
				// 	  request->getParam("audio_host")->value().toCharArray((char *)buff, 250);
				// 	  if (audioHost)
				// 	  {
				// 		  free(audioHost);
				// 	  }
				// 	  audioHost = strdup((char *)buff);
				// 	  set_data_to_csv("audio_host", (char *)buff);
				// 	  free(buff);
				// 	// audio.connecttohost(audioHost); //  128k mp3

				//   }
				//   if (request->hasParam("audio_volume"))
				//   {
				// 	  uint8_t *buff;
				// 	  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
				// 	  request->getParam("audio_volume")->value().toCharArray((char *)buff, 50);
				// 	  audioVolume = atoi((char *)buff);
				// 	  set_data_to_csv("audio_volume", (char *)buff);
				// 	  free(buff);
				// 		// audio.setVolume(audioVolume); // 0...21
					  
				//   }
				  request->send(SPIFFS, "/ApIndex.html", String(), false, processor);
				  //   request->send(SPIFFS, "/index.html", String(), false, processor);
				  Serial.println("Client Here !"); });

	// 	server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){

	// 		Serial.println("Post req");
	// 		if (request->hasParam("orca_ip"))
	// 		{
	// 			Serial.printf("Orca ip :", request->getParam("orca_ip"));
	// 		}
	// 		request->send(SPIFFS, "/ApIndex.html", String(), false, processor);
	// //   request->send(SPIFFS, "/index.html", String(), false, processor);
	// 		// Serial.println("Client Here !");
	// 	});
	server.begin();
}

void setup_server_for_sta()
{

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (request->hasParam("orca_ip"))
				  {

					  // AsyncWebParameter* p = request->getParam("orca_ip");
					  //   Serial.print("Orca ip :");
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("orca_ip")->value().toCharArray((char *)buff, 50);
					  //   Serial.println(request->getParam("orca_ip")->value().toCharArray());
					  orca_ip = IPAddress(get_octet((char *)buff, 1), get_octet((char *)buff, 2), get_octet((char *)buff, 3), get_octet((char *)buff, 4));

					  Serial.println(orca_ip.toString());

					  set_data_to_csv("orca_ip", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("orca_port"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("orca_port")->value().toCharArray((char *)buff, 50);
					  orca_port = atoi((char *)buff);
					  set_data_to_csv("orca_port", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("usine_ip"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("usine_ip")->value().toCharArray((char *)buff, 50);
					  //   Serial.println(request->getParam("orca_ip")->value().toCharArray());
					  usine_ip = IPAddress(get_octet((char *)buff, 1), get_octet((char *)buff, 2), get_octet((char *)buff, 3), get_octet((char *)buff, 4));

					  Serial.println(usine_ip.toString());

					  set_data_to_csv("usine_ip", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("usine_port"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("usine_port")->value().toCharArray((char *)buff, 50);
					  usine_port = atoi((char *)buff);
					  set_data_to_csv("usine_port", (char *)buff);
					  free(buff);
				  }

				  if (request->hasParam("tripode_id"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("tripode_id")->value().toCharArray((char *)buff, 50);
					  if (tripode_id)
					  {
						  free(tripode_id);
					  }
					  tripode_id = strdup((char *)buff);
					  set_data_to_csv("tripode_id", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("fractal_state_pos_x"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("fractal_state_pos_x")->value().toCharArray((char *)buff, 50);
					  fractal_state_pos_x = atoi((char *)buff);
					  set_data_to_csv("fractal_state_pos_x", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("fractal_state_pos_y"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("fractal_state_pos_y")->value().toCharArray((char *)buff, 50);
					  fractal_state_pos_y = atoi((char *)buff);
					  set_data_to_csv("fractal_state_pos_y", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("glyph_pos_x"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("glyph_pos_x")->value().toCharArray((char *)buff, 50);
					  glyph_pos_x = atoi((char *)buff);
					  set_data_to_csv("glyph_pos_x", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("glyph_pos_y"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("glyph_pos_y")->value().toCharArray((char *)buff, 50);
					  glyph_pos_y = atoi((char *)buff);
					  set_data_to_csv("glyph_pos_y", (char *)buff);
					  free(buff);
				  }

				  if (request->hasParam("sta_ssid"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("sta_ssid")->value().toCharArray((char *)buff, 50);
					  if (ssid)
					  {
						  free(ssid);
					  }
					  ssid = strdup((char *)buff);
					  set_data_to_csv("sta_ssid", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("sta_password"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("sta_password")->value().toCharArray((char *)buff, 50);
					  if (password)
					  {
						  free(password);
					  }
					  password = strdup((char *)buff);
					  set_data_to_csv("sta_password", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("ap_ssid"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("ap_ssid")->value().toCharArray((char *)buff, 50);
					  if (APssid)
					  {
						  free(APssid);
					  }
					  APssid = strdup((char *)buff);
					  set_data_to_csv("ap_ssid", (char *)buff);
					  free(buff);
				  }
				  if (request->hasParam("ap_password"))
				  {
					  uint8_t *buff;
					  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
					  request->getParam("ap_password")->value().toCharArray((char *)buff, 50);
					  if (APpassword)
					  {
						  free(APpassword);
					  }
					  APpassword = strdup((char *)buff);
					  set_data_to_csv("ap_password", (char *)buff);
					  free(buff);
				  }
	// 			  if (request->hasParam("audio_host"))
	// 			  {
	// 				  uint8_t *buff;
	// 				  buff = (uint8_t *)malloc(sizeof(uint8_t) * 250);
	// 				  request->getParam("audio_host")->value().toCharArray((char *)buff, 250);
	// 				  if (audioHost)
	// 				  {
	// 					  free(audioHost);
	// 				  }
	// 				  audioHost = strdup((char *)buff);
	// 				  set_data_to_csv("audio_host", (char *)buff);
	// 				  free(buff);
	// // audio.connecttohost(audioHost); //  128k mp3

	// 			  }
	// 			  if (request->hasParam("audio_volume"))
	// 			  {
	// 				  uint8_t *buff;
	// 				  buff = (uint8_t *)malloc(sizeof(uint8_t) * 50);
	// 				  request->getParam("audio_volume")->value().toCharArray((char *)buff, 50);
	// 				  audioVolume = atoi((char *)buff);
	// 				  set_data_to_csv("audio_volume", (char *)buff);
	// 				  free(buff);
	// // audio.setVolume(audioVolume); // 0...21
	// 			  }


				  request->send(SPIFFS, "/StaIndex.html", String(), false, processor);
				  //   request->send(SPIFFS, "/index.html", String(), false, processor);
				  Serial.println("Client Here !"); });
	server.begin();
}

void setup_credentials()
{
	char *tmp = 0;

	if (tmp = get_data_from_csv("orca_ip"))
	{
		orca_ip = IPAddress(get_octet((char *)tmp, 1), get_octet((char *)tmp, 2), get_octet((char *)tmp, 3), get_octet((char *)tmp, 4));
		free(tmp);
	}
	else
	{
		orca_ip = IPAddress(0, 0, 0, 0);
	}

	if (tmp = get_data_from_csv("orca_port"))
	{
		orca_port = atoi(tmp);
		free(tmp);
	}
	else
	{
		orca_port = 49160;
	}

	if (tmp = get_data_from_csv("usine_ip"))
	{

		usine_ip = IPAddress(get_octet((char *)tmp, 1), get_octet((char *)tmp, 2), get_octet((char *)tmp, 3), get_octet((char *)tmp, 4));
		free(tmp);
	}
	else
	{
		usine_ip = IPAddress(0, 0, 0, 0);
	}

	if (tmp = get_data_from_csv("usine_port"))
	{
		usine_port = atoi(tmp);
		free(tmp);
	}
	else
	{
		usine_port = 2002;
	}

	if ((tripode_id = get_data_from_csv("tripode_id")) == 0)
	{
		tripode_id = strdup("1_1");
	}

	if (tmp = get_data_from_csv("fractal_state_pos_x"))
	{
		fractal_state_pos_x = atoi(tmp);
		free(tmp);
	}

	if (tmp = get_data_from_csv("fractal_state_pos_y"))
	{
		fractal_state_pos_y = atoi(tmp);
		free(tmp);
	}

	if (tmp = get_data_from_csv("glyph_pos_x"))
	{
		glyph_pos_x = atoi(tmp);
		free(tmp);
	}

	if (tmp = get_data_from_csv("glyph_pos_y"))
	{
		glyph_pos_y = atoi(tmp);
		free(tmp);
	}

	if ((ssid = get_data_from_csv("sta_ssid")) == 0)
	{
		ssid = strdup("tripodesAP");
	}

	if ((password = get_data_from_csv("sta_password")) == 0)
	{
		password = strdup("44448888");
	}

	if ((APssid = get_data_from_csv("ap_ssid")) == 0)
	{
		APssid = strdup("tripodesAP");
	}

	if ((APpassword = get_data_from_csv("ap_password")) == 0)
	{
		APpassword = strdup("44448888");
	}

	// if ((audioHost = get_data_from_csv("audio_host")) == 0)
	// {
	// 	audioHost = strdup("http://icecast.radiofrance.fr/francemusique-hifi.aac");
	// }

	// if (tmp = get_data_from_csv("audio_volume"))
	// {
	// 	audioVolume = atoi(tmp);
	// 	free(tmp);
	// }

	// Serial.printf("Sta Ssid (csv) : \'%s\'\n", get_data_from_csv("sta_ssid"));
	// Serial.printf("Sta Password (csv) : \'%s\'\n", get_data_from_csv("sta_password"));
	// Serial.printf("Ap Ssid (csv) : \'%s\'\n", get_data_from_csv("ap_ssid"));
	// Serial.printf("Ap Password (csv) : \'%s\'\n", get_data_from_csv("ap_password"));
}

// Setup Motors, Udp here
void ap_setup()
{
	WiFi.mode(WIFI_AP);
	WiFi.softAP(APssid, APpassword, 1, 0, 10);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	tft.printf("AP addr: %s\n", myIP.toString().c_str());
	setup_server_for_ap();
	ledcSetup(motorChannel1, motorFreq, motorResolution);
	ledcSetup(motorChannel2, motorFreq, motorResolution);
	ledcSetup(motorChannel3, motorFreq, motorResolution);
	ledcAttachPin(MOTOR_1, motorChannel1);
	ledcAttachPin(MOTOR_2, motorChannel2);
	ledcAttachPin(MOTOR_3, motorChannel3);
	Udp.begin(localUdpPort);
	// setup_bone_conducer();

	xTaskCreatePinnedToCore(update_sta_list, "update_sta_list", 10000, NULL, 1, &Task1, 1);
}

void sta_setup()
{
	// Set your Static IP address
	// Set your Gateway IP address
	// IPAddress gateway(10, 0, 1, 1);

	// IPAddress subnet(255, 255, 0, 0);

	WiFi.mode(WIFI_STA);

	// Static ip attributon
	//  WiFi.config(local_IP, gateway, subnet);

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
		if (millis() - timeStamp > 60000)
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
	setup_server_for_sta();
	Udp.begin(localUdpPort);
	Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
	Serial.println("Setting up audio flux");
	// setup_bone_conducer();
	Serial.println("Audio flux Ok");
}

void setup()
{
	EEPROM.begin(EEPROM_SIZE);
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

	// put your setup code here, to run once:
	current_mode = NONE_MODE;
	Serial.begin(115200);
	if (!SPIFFS.begin())
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		tft.printf("An Error has occurred while mounting SPIFFS");
	}
	// WiFi.mode(WIFI_STA);
	// WiFi.begin(ssid, password);

	setup_credentials();

	Wire.begin();

	// oscAddress = EEPROM.readUInt(0);
	// current_mode = GYRO_MODE;

	// delay(500);
	if (!gyro.init())
	{
		Serial.println("Failed to autodetect gyro type!");
		tft.printf("Failed to autodetect gyro type!");
		while (1)
			;
	}
	gyro.enableDefault();
	// else
	// {
	// 	while (1)
	// 	{
	// 		gyro.read();
	// 		Serial.printf("Gyro : %d | %d | %d\n", gyro.g.x, gyro.g.y, gyro.g.z);
	// 		delay(100);
	// 	}
	// }

	timers[3] = timerBegin(3, 80, true);
	timerAttachInterrupt(timers[3], &call_buttons, false);
	timerAlarmWrite(timers[3], 50 * 1000, true);
	timerAlarmEnable(timers[3]);
	button_init();

	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);

	tft.printf("Please selected your \nmode \n(with bottom buttons)");
	tft.setCursor(0, 230);
	tft.setTextColor(TFT_RED);
	tft.printf("AP mode");
	tft.setCursor(85, 230);
	tft.setTextColor(TFT_BLUE);
	tft.printf("STA mode");
	tft.setCursor(0, 0);

	server.serveStatic("/tripode.ico", SPIFFS, "/tripode.ico");
	server.serveStatic("/main.css", SPIFFS, "/main.css");

	for (;;)
	{
		Serial.print("tour de boucle :");
		Serial.println(current_mode);
		if (current_mode & STA_MASK)
		{
			sta_setup();
			break;
		}
		else if (current_mode & AP_MASK)
		{
			ap_setup();
			break;
		}
		delay(100);
	}
}

void drawMotorsActivity2()
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
	float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);
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

	// uint32_t color1 = TFT_BLUE;
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

	// drawing_sprite.drawCircle(TFT_WIDTH / 2, TFT_HEIGHT/4 * i + TFT_HEIGHT/4 , 20, TFT_BLUE);
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

// create sta list instant

void drawNetworkActivity(bool is_udp_sending, bool is_osc_sending)
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
	float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);
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

	// wifi_sta_list_t wifi_sta_list;
	// tcpip_adapter_sta_list_t adapter_sta_list;

	// memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
	// memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));

	// esp_wifi_ap_get_sta_list(&wifi_sta_list);
	// tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);

	// for (int i = 0; i < adapter_sta_list.num; i++)
	// {

	// 	drawing_sprite.setTextColor(TFT_YELLOW);

	// 	drawing_sprite.setCursor(0, 50 + (i * 14));

	// 	// drawing_sprite.println("");

	// 	tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];

	// 	//drawing_sprite.setTextColor(TFT_BLUE);

	// 	// drawing_sprite.print("MAC: ");
	// 	// drawing_sprite.setTextColor(TFT_WHITE);

	// 	// for(int i = 0; i< 6; i++){

	// 	// 	drawing_sprite.printf("%02X", station.mac[i]);
	// 	// 	if(i<5)drawing_sprite.print(":");
	// 	// }

	// 	drawing_sprite.setTextColor(TFT_BLUE);

	// 	drawing_sprite.print("\nIP:  ");
	// 	drawing_sprite.setTextColor(TFT_WHITE);

	// 	drawing_sprite.println(ip4addr_ntoa(&(station.ip))); //leak?
	// }
	{
		int i = 0;
		sta_list_mutex.lock();
		for (const t_sta_list &elem : sta_list)
		{
			drawing_sprite.setTextColor(TFT_YELLOW);

			drawing_sprite.setCursor(0, 50 + (i * 14));
			drawing_sprite.setTextColor(TFT_BLUE);

			drawing_sprite.print("\nIP:  ");
			drawing_sprite.setTextColor(TFT_WHITE);
			drawing_sprite.println(elem.ip_adress);

			i++;
		}
		sta_list_mutex.unlock();
	}

	drawBatteryLevel(&drawing_sprite, 100, 00, battery_voltage);
	drawUpdSendingActivity(&drawing_sprite, is_udp_sending, is_osc_sending);
	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();
	// sta_list_mutex.lock();
	// // for (const auto &elem : sta_list)
	// // {
	// // 	Serial.print("Ip : ");
	// // 	Serial.print(elem.ip_adress);
	// // 	Serial.print(" has website : ");
	// // 	Serial.println(elem.has_website);
	// // }
	// sta_list_mutex.unlock();
}

void set_pwm0(int pwm, int tonality)
{
	ledcSetup(motorChannel1, tonality, motorResolution);
	ledcWrite(motorChannel1, pwm);
}

void set_pwm1(int pwm, int tonality)
{
	ledcSetup(motorChannel2, tonality, motorResolution);
	ledcWrite(motorChannel2, pwm);
}

void set_pwm2(int pwm, int tonality)
{
	ledcSetup(motorChannel3, tonality, motorResolution);
	ledcWrite(motorChannel3, pwm);
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
			// t_data_task dataTask;

			int duration = convertedPacket.substring(convertedPacket.indexOf("D") + 1).toInt();
			int intensity = convertedPacket.substring(convertedPacket.indexOf("I") + 1).toInt();
			int pin = convertedPacket.substring(convertedPacket.indexOf("P") + 1).toInt();
			int tonality = convertedPacket.substring(convertedPacket.indexOf("T") + 1).toInt();
			if (pin <= 2 && pin >= 0)
			{
				timers_end[pin] = esp_timer_get_time() / 1000 + duration;
				g_set_pwm[pin](intensity / 5, tonality);
				pwmValues[pin] = intensity / 5;
			}
		}
	}
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

void sendOscFloatMessage(char *oscPrefix, float oscMessage, const IPAddress ipOut, const uint32_t portOut)
{
	char *tmpOscPrefix = 0;
	tmpOscPrefix = (char *)malloc(sizeof(char) * (strlen(oscPrefix) + 10));

	strcpy(tmpOscPrefix, oscPrefix);
	strcat(tmpOscPrefix, "_");
	strcat(tmpOscPrefix, tripode_id);
	// OSCMessage msg(oscPrefix);
	OSCMessage msg(tmpOscPrefix);

	// String message = String(accel_event.acceleration.x, 3)
	// 		+ " " + String(accel_event.acceleration.y, 3)
	// 		+ " " + String(accel_event.acceleration.z, 3);
	// String message = "Hello world" ;
	// char messageBuffer[50];
	// oscMessage.toCharArray(messageBuffer, 50);
	// Serial.println(oscMessage);
	msg.add(oscMessage);
	// Udp.beginPacket(outIp, outPort);
	Udp.beginPacket(ipOut, portOut);
	msg.send(Udp);
	Udp.endPacket();
	msg.empty();
	free(tmpOscPrefix);
}

void sendUpdMessage(const char *buffer, const IPAddress ipOut, const uint32_t portOut)
{
	// Udp.beginPacket(ipOut, outPort);
	Udp.beginPacket(ipOut, portOut);
	// Udp.write(buffer, buffer_size);
	Udp.printf(buffer);
	// Udp.write('E');
	Udp.endPacket();
}

void sendUpdAplhaMessage(float alpha, const IPAddress ipOut, const uint32_t portOut)
{
	// Udp.beginPacket(ipOut, outPort);
	Udp.beginPacket(ipOut, portOut);
	// Udp.write(buffer, buffer_size);
	Udp.printf("write:E;0;%d", (int)((alpha)*100));
	// Udp.write('E');
	Udp.endPacket();
	// delay(1000);
}


int convertCharToBase35(char c)
{
	if ((c >= 'A' && c <= 'Z') )
	{
		return (c - 'A' + 10);
	}
	else if (c >= 'a' && c <= 'z')
	{
		return (c - 'a' + 10);
	}
	else if (c >= '0' && c <= '9')
	{
		return (c - '0');
	} 
	return 0;
}

int convertOrcaMidiToHalfTones(char c)
{
	switch (c)
	{
	case 'C':
		return 0;
		break;
	case 'c':
		return 1;
		break;
	case 'D':
		return 2;
		break;
	case 'd':
		return 3;
		break;
	case 'E':
		return 4;
		break;
	case 'e':
		return 4;
		break;
	case 'F':
		return 5;
		break;
	case 'f':
		return 6;
		break;
	case 'G':
		return 7;
		break;
	case 'g':
		return 8;
		break;
	case 'A':
		return 9;
		break;
	case 'a':
		return 10;
		break;
	case 'B':
		return 11;
		break;
	case 'b':
		return 11;
		break;

	default:
		return 0;
		break;
	}
}

char convertBase35ToChar(int nb)
{
	if (nb < 0)
	{
		return ('0');
	}
	if (nb > 35)
	{
		return ('z');
	}
	if (nb < 10)
	{
		return ('0' + nb);
	}
	else
	{
		return ('a' + nb - 10);
	}
}

void sendUdpFractalState(float alpha, const IPAddress ipOut, const uint32_t portOut)
{
	// VABCD
	char letter = ' ';
	if (alpha <= 0.6)
		letter = 'A';
	else if (alpha < 0.9)
		letter = 'B';
	else if (alpha <= 1.1)
		letter = 'C';
	else
		letter = 'D';
	char speed = '9';
	int base35 = (int)fmap(alpha, 0, 1.5, 0, 35);

	Udp.beginPacket(ipOut, portOut);
	Udp.printf("write:2C9\n"
			   " 2%cTE\n"
			   "    \n"
			   "%cR%cJ\n"
			   "  X;%d;%d",
			   speed, convertBase35ToChar(base35), convertBase35ToChar(base35 + 5), fractal_state_pos_x, fractal_state_pos_y);

	// Udp.printf("write:%cR%c\n  XE;%d;%d", convertBase35ToChar(base35), convertBase35ToChar(base35 + 5), fractal_state_pos_x, fractal_state_pos_y);
	Udp.endPacket();
}

void sendUdpXYZ(t_float3 gyro, const IPAddress ipOut, const uint32_t portOut)
{
		// VABCD
	char letter = ' ';
	int base35X = (int)fmap(gyro.x, 0, 40000, 0, 9);
	int base35Y = (int)fmap(gyro.y, 0, 40000, 0, 9);
	int base35Z = (int)fmap(gyro.z, 0, 40000, 0, 9);

	//12
	Udp.beginPacket(ipOut, portOut);
	Udp.printf("write:1V%c2V%c3V%c;0;0\n", convertBase35ToChar(base35X), convertBase35ToChar(base35Y), convertBase35ToChar(base35Z), fractal_state_pos_x, fractal_state_pos_y);
	Udp.endPacket();
}

void sendOrcaLine(char *line, uint16_t x, uint16_t y, const IPAddress ipOut, const uint32_t portOut)
{
	Udp.beginPacket(ipOut, portOut);
	Udp.printf("write:%s;%d;%d", line, x, y);
	Udp.endPacket();
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
	float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (VREF / 1000.0);
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

	compassArraw(tft, &drawing_sprite, 45, 175, (-(int)(heading * 180 / PI)));

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

	float gyroscope = map(sqrtf(gyro.g.x * gyro.g.x + gyro.g.y * gyro.g.y + gyro.g.z * gyro.g.z), 0, 37000, 0, 50);
	float acceleration = map(sqrtf(accel_event.acceleration.x * accel_event.acceleration.x + accel_event.acceleration.y * accel_event.acceleration.y + accel_event.acceleration.z * accel_event.acceleration.z), 0, 40, 0, 50);

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
	Serial.print((int)gyro.g.x);
	Serial.print(" Y: ");
	Serial.print((int)gyro.g.y);
	Serial.print(" Z: ");
	Serial.println((int)gyro.g.z);

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

	float gyro_normal = map(sqrtf(gyro.g.x * gyro.g.x + gyro.g.y * gyro.g.y + gyro.g.z * gyro.g.z), 0, 37000, 0, 100);
	float accel_normal = map(sqrtf(accel_event.acceleration.x * accel_event.acceleration.x + accel_event.acceleration.y * accel_event.acceleration.y + accel_event.acceleration.z * accel_event.acceleration.z), 0, 40, 0, 100);

	if (0)
	{

		// sendOscFloatMessage("/accelerometer_x", (float)accel_x);
		// sendOscFloatMessage("/accelerometer_y", (float)accel_y);
		// sendOscFloatMessage("/accelerometer_z", (float)accel_z);
		// sendOscFloatMessage("/accelerometer_normal", (float)accel_normal);

		// sendOscFloatMessage("/gyroscope_x", (float)gyro_x);
		// sendOscFloatMessage("/gyroscope_y", (float)gyro_y);
		// sendOscFloatMessage("/gyroscope_z", (float)gyro_z);
		// sendOscFloatMessage("/gyroscope_normal", (float)gyro_normal);

		// sendOscFloatMessage("/magnetometer_x", (float)magnet_x);
		// sendOscFloatMessage("/magnetometer_y", (float)magnet_y);
		// sendOscFloatMessage("/magnetometer_z", (float)magnet_z);
	}

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

void update_sensors(t_sensors *sensors)
{
	sensors_event_t accel_event;
	sensors_event_t mag_event;
	accel.getEvent(&accel_event);
	mag.getEvent(&mag_event);
	gyro.read();

	// sensors->accel.x = fmap(accel_event.acceleration.x, -40, 40, -100, 100);
	// sensors->accel.y = fmap(accel_event.acceleration.y, -40, 40, -100, 100);
	// sensors->accel.z = fmap(accel_event.acceleration.z, -40, 40, -100, 100);
	// sensors->gyro.x = fmap(gyro.g.x, -37000, 37000, -100, 100);
	// sensors->gyro.y = fmap(gyro.g.y, -37000, 37000, -100, 100);
	// sensors->gyro.z = fmap(gyro.g.z, -37000, 37000, -100, 100);
	// sensors->mag.x = fmap(mag_event.magnetic.x, -100, 100, -100, 100);
	// sensors->mag.y = fmap(mag_event.magnetic.y, -100, 100, -100, 100);
	// sensors->mag.z = fmap(mag_event.magnetic.z, -100, 100, -100, 100);

	sensors->accel.x = accel_event.acceleration.x;
	sensors->accel.y = accel_event.acceleration.y;
	sensors->accel.z = accel_event.acceleration.z;
	sensors->gyro.x = gyro.g.x;
	sensors->gyro.y = gyro.g.y;
	sensors->gyro.z = gyro.g.z;

	// sensors->gyro.x =42;
	// sensors->gyro.y = 42;
	// sensors->gyro.z = 42;

	sensors->mag.x = mag_event.magnetic.x;
	sensors->mag.y = mag_event.magnetic.y;
	sensors->mag.z = mag_event.magnetic.z;

	// Serial.printf("Acc  : %02f | %02f | %02f\n", sensors->accel.x, sensors->accel.y, sensors->accel.z);
	// Serial.printf("Gyro : %02f | %02f | %02f\n", sensors->gyro.x, sensors->gyro.y, sensors->gyro.z);
	// Serial.printf("Mag  : %02f | %02f | %02f\n", sensors->mag.x, sensors->mag.y, sensors->mag.z);
}

void sendOscMessage(float dfa_value, t_sensors *sensors, const IPAddress ipOut, const uint32_t portOut)
{
	// Udp.beginPacket(, outIp, outPort);

	sendOscFloatMessage("/dfa", (float)dfa_value, ipOut, portOut);

	sendOscFloatMessage("/accelerometer_x", (float)sensors->accel.x, ipOut, portOut);
	sendOscFloatMessage("/accelerometer_y", (float)sensors->accel.y, ipOut, portOut);
	sendOscFloatMessage("/accelerometer_z", (float)sensors->accel.z, ipOut, portOut);
	sendOscFloatMessage("/accelerometer_normal", (float)sqrt(sensors->accel.x * sensors->accel.x + sensors->accel.y * sensors->accel.y + sensors->accel.z * sensors->accel.z), ipOut, portOut);

	sendOscFloatMessage("/gyroscope_x", (float)sensors->gyro.x, ipOut, portOut);
	sendOscFloatMessage("/gyroscope_y", (float)sensors->gyro.y, ipOut, portOut);
	sendOscFloatMessage("/gyroscope_z", (float)sensors->gyro.z, ipOut, portOut);
	sendOscFloatMessage("/gyroscope_normal", (float)sqrt(sensors->gyro.x * sensors->gyro.x + sensors->gyro.y * sensors->gyro.y + sensors->gyro.z * sensors->gyro.z), ipOut, portOut);

	sendOscFloatMessage("/magnetometer_x", (float)sensors->mag.x, ipOut, portOut);
	sendOscFloatMessage("/magnetometer_y", (float)sensors->mag.y, ipOut, portOut);
	sendOscFloatMessage("/magnetometer_z", (float)sensors->mag.z, ipOut, portOut);
}

float updateDFA(t_sensors sensors)
{
	static float *x = 0;
	static float *x_tmp = 0;
	static float *alpha_mean = 0;

	size_t size_x = 400;
	size_t size_tmp_x = 20;
	size_t size_alpha_mean = 50;
	static size_t current_index = 0;
	static size_t current_alpha_index = 0;

	if (x == 0)
	{
		x = (float *)malloc(sizeof(float) * size_x);
		x_tmp = (float *)malloc(sizeof(float) * size_x);
		alpha_mean = (float *)malloc(sizeof(float) * size_alpha_mean);
		for (size_t i = 0; i < size_x; i++)
		{
			x[i] = 0.0;
		}
		for (size_t i = 0; i < size_alpha_mean; i++)
		{
			alpha_mean[i] = 0.0;
		}
	}
	x[current_index] = map(sqrtf(sensors.gyro.x * sensors.gyro.x + sensors.gyro.y * sensors.gyro.y + sensors.gyro.z * sensors.gyro.z), 0, 37000, 0, 100);
	for (size_t i = 0; i < size_x; i++)
	{
		x_tmp[size_x - i - 1] = x[(current_index - i) % size_x];
	}

	for (size_t i = 0; i < size_tmp_x; i++)
	{
		size_t beg_i = map(i, 0, size_tmp_x, 0, size_x);
		float max = 0;
		for (size_t j = 0; j < size_x / size_tmp_x; j++)
		{
			if (max < x_tmp[beg_i + j])
			{
				max = x_tmp[beg_i + j];
			}
		}
		x_tmp[i] = max;
		// Serial.printf("%f, ", max);
	}
	// Serial.printf("\n");

	// Serial.printf("current index : %d\n", current_index);
	// for (size_t i = 0; i < size_x; i++)
	// {
	// 	Serial.printf("x_tmp[%d] : %f ", i, x_tmp[i]);
	// 	Serial.printf("x[%d] : %f\n", i, x[i]);
	// }

	current_index = current_index >= size_x - 1 ? 0 : current_index + 1;

	// printf("Dfa : %f\n", dfa(x, sizeof(x) / 4, 1, 4, 0.5));

	// float dfa_value = dfa(x_tmp, size_tmp_x, 2, 4.5, 0.5);
	float dfa_value = dfa(x_tmp, size_tmp_x, 1, 4, 0.2);
	dfa_value = abs(dfa_value);

	alpha_mean[current_alpha_index] = dfa_value;
	//
	dfa_value = mean(alpha_mean, size_alpha_mean);

	current_alpha_index = current_alpha_index >= size_alpha_mean ? 0 : current_alpha_index + 1;

	// if (dfa_value >= 0.5 && dfa_value <= 3)
	// {
	// 	dfa_value = 0.4342523523;
	// }
	dfa_value = fmap(dfa_value, 0, 18, 0, 1.5);
	// dfa_value = dfa_value * dfa_value;
	// Serial.printf("Alpha : %f\n", dfa_value);

	if (SCALE_DFA == 1)
	{
		if (mean(x_tmp, size_tmp_x) < SCALE_DFA_TRESHOLD)
		{
			dfa_value /= fmap(mean(x_tmp, size_tmp_x), 0, SCALE_DFA_TRESHOLD, 6, 1);
		}
	}

	return (dfa_value);
	// return (dfa(x_tmp, size_x, 4, 10, 0.1));
	// dfa(x, sizeof(x) / 4, 2, 4.5, 0.5));
}

// void sendOrcaLine(char *line, uint16_t x, uint16_t y, uint16_t size, const IPAddress ipOut, const uint32_t portOut)
void drawInOrca(float alpha, const IPAddress ipOut, const uint32_t portOut)
{

	if (alpha <= 0.6)
	{
		sendOrcaLine("  R                                                \n"
					 "2X                                                 \n"
					 "                                                   \n"
					 "  CO                                               \n"
					 "   8G  X                                           \n",
					 glyph_pos_x, glyph_pos_y, ipOut, portOut);
		sendOrcaLine("      #XX#                    #XXXXXX#    #XXXXXX# \n"
					 "      #XX#                    #XXXXXX#    #XXXXXX# \n"
					 "      #XX#                    #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "      #XX#                    #XXXXXX#    #XX#     \n"
					 "      #XX#                    #XXXXXX#    #XX#     \n"
					 "      #XX#                    #XXXXXX#    #XX#     \n",
					 glyph_pos_x, glyph_pos_y + 5, ipOut, portOut);
		sendOrcaLine("      #XX#                    #XXXXXX#    #XX#     \n"
					 "      #XX#                    #XXXXXX#    #XX#     \n"
					 "      #XX#                    #XXXXXX#    #XX#     \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "      #XXXXXX#                #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#                #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#                #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n"
					 "      #XXXXXX#                    #XX#    #XXXXXX# \n",
					 glyph_pos_x, glyph_pos_y + 17, ipOut, portOut);
	}
	else if (alpha <= 0.9)
	{
		sendOrcaLine("  R                                                \n"
					 "2X                                                 \n"
					 "                                                   \n"
					 "  CO                                               \n"
					 "   8G  X                                           \n",
					 glyph_pos_x, glyph_pos_y, ipOut, portOut);
		sendOrcaLine("      #XX#        #XX#        #XXXXXX#    #XXXXXX# \n"
					 "      #XX#        #XX#        #XXXXXX#    #XXXXXX# \n"
					 "      #XX#        #XX#        #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "      #XX#        #XXXXXX#                #XX#     \n"
					 "      #XX#        #XXXXXX#                #XX#     \n"
					 "      #XX#        #XXXXXX#                #XX#     \n",
					 glyph_pos_x, glyph_pos_y + 5, ipOut, portOut);
		sendOrcaLine("      #XX#        #XXXXXX#                #XX#     \n"
					 "      #XX#        #XXXXXX#                #XX#     \n"
					 "      #XX#        #XXXXXX#                #XX#     \n"
					 "      #XXXXXX#        #XX#                #XXXXXX# \n"
					 "      #XXXXXX#        #XX#                #XXXXXX# \n"
					 "      #XXXXXX#        #XX#                #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#                #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#                #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#                #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#                #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#                #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#                #XXXXXX# \n",
					 glyph_pos_x, glyph_pos_y + 17, ipOut, portOut);
	}
	else if (alpha <= 1.1)
	{
		sendOrcaLine("  R                                                \n"
					 "2X                                                 \n"
					 "                                                   \n"
					 "  CO                                               \n"
					 "   8G  X                                           \n",
					 glyph_pos_x, glyph_pos_y, ipOut, portOut);
		sendOrcaLine("                  #XX#        #XXXXXX#    #XXXXXX# \n"
					 "                  #XX#        #XXXXXX#    #XXXXXX# \n"
					 "                  #XX#        #XXXXXX#    #XXXXXX# \n"
					 "                  #XXXXXX#        #XX#    #XXXXXX# \n"
					 "                  #XXXXXX#        #XX#    #XXXXXX# \n"
					 "                  #XXXXXX#        #XX#    #XXXXXX# \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n",
					 glyph_pos_x, glyph_pos_y + 5, ipOut, portOut);
		sendOrcaLine("      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XXXXXX#        #XX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#        #XX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#        #XX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#    #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#    #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#    #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n",
					 glyph_pos_x, glyph_pos_y + 17, ipOut, portOut);
	}
	else
	{
		sendOrcaLine("  R                                                \n"
					 "2X                                                 \n"
					 "                                                   \n"
					 "  CO                                               \n"
					 "   8G  X                                           \n",
					 glyph_pos_x, glyph_pos_y, ipOut, portOut);
		sendOrcaLine("      #XX#        #XX#        #XXXXXX#    #XXXXXX# \n"
					 "      #XX#        #XX#        #XXXXXX#    #XXXXXX# \n"
					 "      #XX#        #XX#        #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "                                          #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n",
					 glyph_pos_x, glyph_pos_y + 5, ipOut, portOut);
		sendOrcaLine("      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n"
					 "      #XXXXXX#        #XX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#        #XX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#        #XX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#    #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#    #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#    #XXXXXX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n"
					 "      #XXXXXX#    #XXXXXX#        #XX#    #XXXXXX# \n",
					 glyph_pos_x, glyph_pos_y + 17, ipOut, portOut);
	}
}

void udpInitOrca(const IPAddress ipOut, const uint32_t portOut)
{
	// sendOrcaLine("              #XX#        #XXXXXX#    #XXXXXX#    #XX#     \n", 20, 0, ipOut, portOut);

	for (uint16_t y = 0; y < 100; y++)
	{
		Udp.beginPacket(ipOut, portOut);
		Udp.printf("write:;#%dD300I255P1;%d;%d", y % 3, 20, y + 10);
		Udp.endPacket();
	}
}

void play_midi_notes()
{
	int packetSize = Udp.parsePacket();
	if (packetSize)
	{
		int len = Udp.read(incomingPacket, 255);
		if (len > 0)
		{
			incomingPacket[len] = 0;
		}
		if (len > 4)
		{
			int16_t channel = convertCharToBase35(incomingPacket[0]);
			int16_t octave = convertCharToBase35(incomingPacket[1]);
			int16_t note = convertOrcaMidiToHalfTones(incomingPacket[2]);
			int16_t velocity = convertCharToBase35(incomingPacket[3]);
			int16_t length = convertCharToBase35(incomingPacket[4]);
			Serial.printf("C %d O %d N %d V %d L %d\n", channel, octave, note, velocity, length);
		}
	}
}

void loop()
{
	t_sensors sensors;
	// if (current_mode & NORM_MASK)
	if (1)
	{

		if ((current_mode & MODE_MASK) == MIDI_MODE)
		{
			play_midi_notes();
		}
		else
		{
			look_for_udp_message();
		}

		for (int i = 0; i < 3; i++)
		{
			if (timers_end[i] != 0 && timers_end[i] < esp_timer_get_time() / 1000)
			{
				g_set_pwm[i](0, 0);
				timers_end[i] = 0;
				pwmValues[i] = 0;
			}
		}

		if (!i2s_on)
		{
			update_sensors(&sensors);
		}

		float dfa_value = updateDFA(sensors);
		// const IPAddress usine_ip(192,168,100,100);
		// const unsigned int usine_port = 2002;          // remote port to receive OSC
		if (osc_sending)
		{
			sendOscMessage(dfa_value, &sensors, usine_ip, usine_port);
		}

		// sendUpdMessage((const char*)"write:E;18;8", orca_ip, orca_port);
		static uint16_t loop_counter = 0;
		if (udp_sending)
		{
			if (loop_counter % updMessageRate == 0)
			{
				sendUpdMessage("select:0;0", orca_ip, orca_port);
				sendUdpXYZ(sensors.gyro,  orca_ip, orca_port);
				Serial.println("Gyrosended");
				
				// sendUdpFractalState(dfa_value, orca_ip, orca_port);
				// sendUpdAplhaMessage(dfa_value, orca_ip, orca_port);
			}
			if (loop_counter % updDrawRate == 0 && UDP_DRAWING == 1)
			{
				//  udpInitOrca(orca_ip, orca_port);
				// drawInOrca(dfa_value, orca_ip, orca_port);
			}
			loop_counter = loop_counter > 1000 ? 0 : loop_counter + 1;
		}

		if ((current_mode & MODE_MASK) == STD_MODE)
		{

			drawMotorsActivity(tft, pwmValues, localUdpPort, ssid, udp_sending, osc_sending);
		}
		else if ((current_mode & MODE_MASK) == MIDI_MODE)
		{
			drawMidiActivity(tft, pwmValues, localUdpPort, ssid, udp_sending, osc_sending);
		}
		else if ((current_mode & MODE_MASK) == SENSORS_MODE)
		{
			drawSensorsActivity(tft, sensors, oscAddress, udp_sending, osc_sending);
		}
		else if ((current_mode & MODE_MASK) == DFA_MODE)
		{
			drawAlpha(tft, dfa_value, udp_sending, osc_sending);
		}
		else if ((current_mode & MODE_MASK) == RUNE_MODE)
		{
			drawRunes(tft, dfa_value, udp_sending, osc_sending);
		}
		else if ((current_mode & MODE_MASK) == AP_MODE)
		{
			drawNetworkActivity(udp_sending, osc_sending);
			// web();
		}
	}
	// Serial.println(".");
	delay(25);
	// delay(500);
	// put your main code here, to run repeatedly:
}