#include "tripodes.h"

void compassArraw(TFT_eSPI tft, TFT_eSprite * sprite, int x, int y, float angle)
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

void drawUpdSendingActivity(TFT_eSprite *sprite)
{
	(*sprite).fillRect(0, 238, 135, 2, TFT_RED);
}

void drawMotorsActivity(TFT_eSPI tft, int32_t pwmValues[3], int32_t localUdpPort, const char *ssid, bool is_upd_sending)
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
	if (WiFi.getMode() == WIFI_MODE_AP)
	{
		drawing_sprite.printf("%s\n\n", WiFi.softAPIP().toString().c_str());
	}
	else
	{
		drawing_sprite.printf("%s\n\n", WiFi.localIP().toString().c_str());
	}

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Udp port : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%d\n\n", localUdpPort);

	// %s\n\nIp : %s\n\nUdp port : %d\n\n", ssid,WiFi.localIP().toString().c_str(), localUdpPort);

	drawing_sprite.setTextColor(TFT_RED);
	drawing_sprite.printf("Tripode Id : ");
	drawing_sprite.setTextColor(TFT_WHITE);
	drawing_sprite.printf("%s\n", TRIPODE_ID);
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
	if (is_upd_sending)
	{
		drawUpdSendingActivity(&drawing_sprite);
	}
	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();
}

void drawSensorsActivity(TFT_eSPI tft, t_sensors sensors, int32_t oscAddress, bool is_upd_sending)
{
	static bool isCalibrated = false;
	static int calMinX = 0;
	static int calMaxX = 0;
	static int calMinY = 0;
	static int calMaxY = 0;


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


	// sensors_event_t accel_event;
	// sensors_event_t mag_event;
	// accel.getEvent(&accel_event);
	// mag.getEvent(&mag_event);
	// // mag.getSensor(&event);
	// // gyro.getEvent(&event);
	// gyro.read();


	// Calculate the angle of the vector y,x

	// Normalize to 0-360


	if (isCalibrated == false)
	{
		calMinX = sensors.mag.x;
		calMaxX = sensors.mag.x + 1;
		calMinY = sensors.mag.y;
		calMaxY = sensors.mag.y + 1;
		isCalibrated = true;
	}
	else
	{
		if (sensors.mag.x < calMinX)
			calMinX = sensors.mag.x;
		else if (sensors.mag.x > calMaxX)
			calMaxX = sensors.mag.x;
		if (sensors.mag.y < calMinY)
			calMinY = sensors.mag.y;
		else if (sensors.mag.y > calMaxY)
			calMaxY = sensors.mag.y;
	}


	int mag_xcal = map(sensors.mag.x, calMinX, calMaxX, -1000, 1000);
	int mag_ycal = map(sensors.mag.y, calMinY, calMaxY, -1000, 1000);
	float heading = atan2((double)mag_xcal, (double)mag_ycal);
	// if (heading < 0) {
	// 	heading = 360 + heading;
	// }

	compassArraw(tft, &drawing_sprite, 45, 175, (-(int)(heading * 180 / PI)));

	// drawing_sprite.setCursor(2, 15);
	// drawing_sprite.printf("osc addr : /%d", oscAddress);


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

	drawCursors(&drawing_sprite, 0, 60, 12, 100, -40, 40, sensors.accel.x, TFT_RED);
	drawCursors(&drawing_sprite, 15, 60, 12, 100, -40, 40, sensors.accel.y, TFT_RED);
	drawCursors(&drawing_sprite, 30, 60, 12, 100, -40, 40, sensors.accel.z, TFT_RED);

	drawCursors(&drawing_sprite, 45, 60, 12, 100, -37000, 37000, sensors.gyro.x, TFT_RED);
	drawCursors(&drawing_sprite, 60, 60, 12, 100, -37000, 37000, sensors.gyro.y, TFT_RED);
	drawCursors(&drawing_sprite, 75, 60, 12, 100, -37000, 37000, sensors.gyro.z, TFT_RED);

	drawCursors(&drawing_sprite, 90, 60, 12, 100, -1000, 1000, mag_xcal, TFT_RED);
	drawCursors(&drawing_sprite, 105, 60, 12, 100, -1000, 1000, mag_ycal, TFT_RED);
	drawCursors(&drawing_sprite, 120, 60, 12, 100, -200, 200, sensors.mag.z, TFT_RED);
	// event.acceleration.
	
	float gyroscope = map(sqrtf(sensors.gyro.x * sensors.gyro.x \
		 + sensors.gyro.y * sensors.gyro.y \
		 + sensors.gyro.z * sensors.gyro.z), 0, 37000, 0, 50);
	float acceleration = map(sqrtf(sensors.accel.x * sensors.accel.x \
		 + sensors.accel.y * sensors.accel.y \
		 + sensors.accel.z * sensors.accel.z), 0, 40, 0, 50);
	
	drawing_sprite.setCursor(5, 170);
	drawing_sprite.printf("Accel");
	drawing_sprite.fillRect(10, 235 - acceleration, 20, acceleration, TFT_RED);
	drawing_sprite.drawRect(10, 185, 20, 50, TFT_WHITE);
	
	drawing_sprite.setCursor(108, 170);
	drawing_sprite.printf("Gyro");
	drawing_sprite.fillRect(110, 235 - gyroscope, 20, gyroscope, TFT_RED);
	drawing_sprite.drawRect(110, 185, 20, 50, TFT_WHITE);

	
	Serial.print("G ");
	Serial.print("X: ");
	Serial.print((int) sensors.gyro.x);
	Serial.print(" Y: ");
	Serial.print((int) sensors.gyro.y);
	Serial.print(" Z: ");
	Serial.println((int) sensors.gyro.z);

	if (is_upd_sending)
	{
		drawUpdSendingActivity(&drawing_sprite);
	}

	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();


	float accel_x = fmap(sensors.accel.x, -40, 40, -100, 100);
	float accel_y = fmap(sensors.accel.y, -40, 40, -100, 100);
	float accel_z = fmap(sensors.accel.z, -40, 40, -100, 100);
	float gyro_x = fmap(sensors.gyro.x, -37000, 37000, -100, 100);
	float gyro_y = fmap(sensors.gyro.y, -37000, 37000, -100, 100);
	float gyro_z = fmap(sensors.gyro.z, -37000, 37000, -100, 100);
	float magnet_x = fmap(sensors.mag.x, -100, 100, -100, 100);
	float magnet_y = fmap(sensors.mag.y, -100, 100, -100, 100);
	float magnet_z = fmap(sensors.mag.z, -100, 100, -100, 100);

	float gyro_normal = map(sqrtf(sensors.gyro.x * sensors.gyro.x \
		 + sensors.gyro.y * sensors.gyro.y \
		 + sensors.gyro.z * sensors.gyro.z), 0, 37000, 0, 100);
	float accel_normal = map(sqrtf(sensors.accel.x * sensors.accel.x \
		 + sensors.accel.y * sensors.accel.y \
		 + sensors.accel.z * sensors.accel.z), 0, 40, 0, 100);





}

void printSign(TFT_eSprite *drawing_sprite, float alpha)
{
	int16_t space = 12;
	int16_t offset = 50;

	if (alpha <= 0.6)
	{
		(*drawing_sprite).setCursor(0, offset);
		(*drawing_sprite).printf("     ▐▌         ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 1);
		(*drawing_sprite).printf("     ▐▌         ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 2);
		(*drawing_sprite).printf("     ▐▌ uniform ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 3);
		(*drawing_sprite).printf("     ▐▌         ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 4);
		(*drawing_sprite).printf("     ▐▌         ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 5);
		(*drawing_sprite).printf("     ▐▌  a:%.2f ╟▌    ", alpha);
		(*drawing_sprite).setCursor(0, offset + space * 6);
		(*drawing_sprite).printf("     ▐▌         ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 7);
		(*drawing_sprite).printf("╓▄▄▄▄╫█▄▄▄▄▄▄▄▄▄█▄▄▄▄▄");
		(*drawing_sprite).setCursor(0, offset + space * 8);
		(*drawing_sprite).printf("     ▐▌         ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 9);
		(*drawing_sprite).printf("     ▐▌         ╟▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 10);
		(*drawing_sprite).printf("     ▐█▄▄▄▄▄▄▄▄▄█▌    ");
	}
	else if (alpha < 0.9)
	{
		(*drawing_sprite).setCursor(0, offset);
		(*drawing_sprite).printf("          ╟█           ");
		(*drawing_sprite).setCursor(0, offset + space * 1);
		(*drawing_sprite).printf("       regular         ");
		(*drawing_sprite).setCursor(0, offset + space * 2);
		(*drawing_sprite).printf("          ╟█           ");
		(*drawing_sprite).setCursor(0, offset + space * 3);
		(*drawing_sprite).printf("        a:%.2f         ", alpha);
		(*drawing_sprite).setCursor(0, offset + space * 4);
		(*drawing_sprite).printf("     █    ╟█    █       ");
		(*drawing_sprite).setCursor(0, offset + space * 5);
		(*drawing_sprite).printf("    █▌    ╟█    ▐█     ");
		(*drawing_sprite).setCursor(0, offset + space * 6);
		(*drawing_sprite).printf("   █▌ █▌  ╟█  ╓█  █▌    ");
		(*drawing_sprite).setCursor(0, offset + space * 7);
		(*drawing_sprite).printf("  █▌   █▌ ╟█ ╙█    █▌  ");
		(*drawing_sprite).setCursor(0, offset + space * 8);
		(*drawing_sprite).printf(" ██     █▌╟█┌█`    ╟█  ");
		(*drawing_sprite).setCursor(0, offset + space * 9);
		(*drawing_sprite).printf("▐█       ████▌      █▌ ");
		(*drawing_sprite).setCursor(0, offset + space * 10);
		(*drawing_sprite).printf("█         █ ▌        █▌");
	}
	else if (alpha <= 1.1)
	{
		(*drawing_sprite).setCursor(0, offset);
		(*drawing_sprite).printf("    ▀█▄      ▄█▀    ");
		(*drawing_sprite).setCursor(0, offset + space * 1);
		(*drawing_sprite).printf("      ▀█▄  ▄█▀      ");
		(*drawing_sprite).setCursor(0, offset + space * 2);
		(*drawing_sprite).printf("        ▀██▀        ");
		(*drawing_sprite).setCursor(0, offset + space * 3);
		(*drawing_sprite).printf("         █▌         ");
		(*drawing_sprite).setCursor(0, offset + space * 4);
		(*drawing_sprite).printf("         █▌         ");
		(*drawing_sprite).setCursor(0, offset + space * 5);
		(*drawing_sprite).printf("        ████        ");
		(*drawing_sprite).setCursor(0, offset + space * 6);
		(*drawing_sprite).printf("      █▀    ▀█      ");
		(*drawing_sprite).setCursor(0, offset + space * 7);
		(*drawing_sprite).printf("▄▄▄▄█▀`       ▀█▄▄▄▄");
		(*drawing_sprite).setCursor(0, offset + space * 8);
		(*drawing_sprite).printf("   █▌  a:%.2f  ▐█   ", alpha);
		(*drawing_sprite).setCursor(0, offset + space * 9);
		(*drawing_sprite).printf("   █▌          ▐█   ");
		(*drawing_sprite).setCursor(0, offset + space * 10);
		(*drawing_sprite).printf("   ▀   fractl   ▀   ");
	}
	else
	{
		(*drawing_sprite).setCursor(0, offset);
		(*drawing_sprite).printf(" ▀█▄             ▄█▀");
		(*drawing_sprite).setCursor(0, offset + space * 1);
		(*drawing_sprite).printf("   ▀█▄ a:%.2f  ▄█▀   ", alpha);
		(*drawing_sprite).setCursor(0, offset + space * 2);
		(*drawing_sprite).printf("    ▀█▄      ▄█▀▀▀▀▀ ");
		(*drawing_sprite).setCursor(0, offset + space * 3);
		(*drawing_sprite).printf("      ▀█▄  ▄█▀      ");
		(*drawing_sprite).setCursor(0, offset + space * 4);
		(*drawing_sprite).printf("          ██       ");
		(*drawing_sprite).setCursor(0, offset + space * 5);
		(*drawing_sprite).printf("       complex       ");
		(*drawing_sprite).setCursor(0, offset + space * 6);
		(*drawing_sprite).printf("          ██        ");
		(*drawing_sprite).setCursor(0, offset + space * 7);
		(*drawing_sprite).printf("          ██        ");
		(*drawing_sprite).setCursor(0, offset + space * 8);
		(*drawing_sprite).printf("          ██        ");
		(*drawing_sprite).setCursor(0, offset + space * 9);
		(*drawing_sprite).printf("     ▄▄█▀            ");
		(*drawing_sprite).setCursor(0, offset + space * 10);
		(*drawing_sprite).printf(" ▄▄█▀▀╙              ");
	}


    //          alpha_sign = "\
 ▀█▄             ▄█▀ \n\
   ▀█▄ "+b +"  ▄█▀   \n\
    ▀█▄      ▄█▀▀▀▀▀ \n\
      ▀█▄  ▄█▀       \n\
        └█"+d+"─         \n\
       " + c + "     \n\
         ╟▌          \n\
         ╟▌          \n\
        "+a+"        \n\
         ╟▌          \n\
     ▄▄█▀"+s+"          \n\
 ▄▄█▀▀╙  "+m+"       "     

//              alpha_sign = "\
           ╟█           \n\
           ╟█           \n\
           ╟█           \n\
           ╟█           \n\
      █    ╟█    █       \n\
     █▌    ╟█    ▐█     \n\
    █▌ █▌  ╟█  ╓█  █    \n\
   █▌   █▌ ╟█ ╙█    █▌  \n\
  ██     █▌╟█┌█`    ╟█  \n\
 ▐█       ████▌      █▌ \n\
╓█         █ ▌        █▌"

	// (*drawing_sprite).printf("eewewfwe");

// 	char *                alpha_sign = "\
//      ▐▌         ╟▌    \n\
//      ▐▌         ╟▌    \n\
//      ▐▌         ╟▌    \n\
//      ▐▌         ╟▌    \n\ //2
//      ▐▌         ╟▌    \n\
//      ▐▌         ╟▌    \n\
//      ▐▌         ╟▌    \n\
// ╓▄▄▄▄╫█▄▄▄▄▄▄▄▄▄█▄▄▄▄▄\n\
//      ▐▌         ╟▌    \n\
//      ▐▌         ╟▌    \n\
//      ▐█▄▄▄▄▄▄▄▄▄█▌    "

}


void drawAlpha(TFT_eSPI tft, float alpha, bool is_upd_sending)
{
	TFT_eSprite drawing_sprite = TFT_eSprite(&tft);
	drawing_sprite.setColorDepth(8);
	drawing_sprite.createSprite(tft.width(), tft.height());

	drawing_sprite.fillSprite(TFT_BLACK);
	drawing_sprite.setTextSize(1);
	drawing_sprite.setTextFont(1);
	drawing_sprite.setTextColor(TFT_WHITE);
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
	drawing_sprite.setCursor(0, 0);



	printSign(&drawing_sprite, alpha);


// 	char *        alpha_sign = "\
    ▀█▄      ▄█▀    \n\n\
      ▀█▄  ▄█▀      \n\n\
        ▀██▀        \n\n\
          ▌         \n\n\
         █▌         \n\n\
        █ ██        \n\n\
      █▀    ▀█      \n\n\
▄▄▄▄█▀`       ▀█▄▄▄▄\n\n\
   █▌          ▐█   \n\n\
   █▌          ▐█   \n\n\
   ▀            ▀   ";
	// drawing_sprite.printf("%s", alpha_sign);
	// // drawing_sprite.printf("\n%d",fmap(sqrtf(sensors.gyro.x * sensors.gyro.x \
	// // 	 + sensors.gyro.y * sensors.gyro.y \
	// // 	 + sensors.gyro.z * sensors.gyro.z), 0, 37000, 0, 100));
	// float gyroscope = map(sqrtf(sensors.gyro.x * sensors.gyro.x \
	// 	 + sensors.gyro.y * sensors.gyro.y \
	// 	 + sensors.gyro.z * sensors.gyro.z), 0, 37000, 0, 100);

	
	// static int32_t last_time = 0;
	// drawing_sprite.printf("\n%d", (int32_t)(esp_timer_get_time() / 1000));
	
	// drawing_sprite.printf("\nlast time :%d", (int32_t)(esp_timer_get_time() / 1000) - last_time);
	// last_time = (esp_timer_get_time() / 1000) ;

	// float alpha = updateDFA(sensors);
	// drawing_sprite.printf("\nDFA : %f", alpha);

	if (alpha <= 0.60)
	{
		Serial.printf("uniform\n");
	}
	else if (alpha < 0.90)
	{
		Serial.printf("regular\n");
	}
	else if (alpha <= 1.1)
	{
		Serial.printf("fractal\n");
	}
	else
	{
		Serial.printf("complex\n");
	}
	if (is_upd_sending)
	{
		drawUpdSendingActivity(&drawing_sprite);
	}
	drawing_sprite.pushSprite(0, 0);
	drawing_sprite.deleteSprite();

}