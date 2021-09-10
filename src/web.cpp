#include "tripodes.h"
#include "FS.h"
#include "SPIFFS.h"

// #define FORMAT_LITTLEFS_IF_FAILED true

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
	Serial.printf("Listing directory: %s\r\n", dirname);

	fs::File root = fs.open(dirname);
	if (!root)
	{
		Serial.println("- failed to open directory");
		return;
	}
	if (!root.isDirectory())
	{
		Serial.println(" - not a directory");
		return;
	}

	fs::File file = root.openNextFile();
	while (file)
	{
		if (file.isDirectory())
		{
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels)
			{
				listDir(fs, file.name(), levels - 1);
			}
		}
		else
		{
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("\tSIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}
void web()
{
	if (!SPIFFS.begin())
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
	}
	// bool formatted = SPIFFS.format();
	// if (formatted)
	// {
	// 	Serial.println("SPIFFS formatted successfully");
	// }
	// else
	// {
	// 	Serial.println("Error formatting");
	// }
	fs::File file = SPIFFS.open("/ApIndex.html");
	if (!file)
	{
		Serial.println("Failed to open file for reading");
		return;
	}
	Serial.print("File size: ");
	Serial.println(file.size());
	uint8_t *buff;
	buff = (uint8_t*)malloc(sizeof(char) * 41);
	while (file.read(buff, 42) > 0)
	{
		buff[40] = '\0';
		Serial.printf("%s", buff);
	}
	
	// file.read()
	file.close();
	// listDir(LITTLEFS, "/", 12);
}