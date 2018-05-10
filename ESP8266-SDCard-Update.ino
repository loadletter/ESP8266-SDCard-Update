#include <SD.h>
#include <SPI.h>

#define UPDATE_BUFF_SIZE 512

const char *update_filename = "update.bin";

void setup()
{
	Serial.begin(9600);
	
	/* init SD */
	Serial.print("SD card: ");
	if (!SD.begin(SS))
	{
		Serial.println("Card initialization failed");
	}
	else
	{
		Serial.println("OK");
		
		if (SD.exists(update_filename))
		{
			updateFromFile();
		}
	}
}

void updateFromFile()
{
	uint8_t buff[UPDATE_BUFF_SIZE];
	size_t data_read;
	
	Serial.print("Update from: ");
	Serial.println(update_filename);
	
	File fwFile = SD.open(update_filename);
	Serial.print("Size is: ");
	Serial.println(fwFile.size());
	
	if (!fwFile)
	{
		Serial.println("Error: couldn't open file");
		return;
	}
	
	if (fwFile.size() < 4)
	{
		Serial.println("Error: file too short");
		fwFile.close();
		return;
	}

	if (fwFile.peek() != 0xE9)
	{
		Serial.println("Error: Magic header does not start with 0xE9");
		fwFile.close();
		return;
	}

	fwFile.seek(3);
	uint32_t bin_flash_size = ESP.magicFlashChipSize((fwFile.peek() & 0xf0) >> 4);
	if (bin_flash_size > ESP.getFlashChipRealSize())
	{
		Serial.println("Error: New binary does not fit SPI Flash size");
		fwFile.close();
		return;
	}
	fwFile.seek(0);
	
	Serial.print("Current sketch size: ");
	Serial.println(ESP.getSketchSize());
	Serial.print("Current free space:  ");
	Serial.println(ESP.getFreeSketchSpace());
	
	uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
	if (!Update.begin(maxSketchSpace))
	{
		Serial.print("Error: ");
		Update.printError(Serial);
		fwFile.close();
		return;
	}
	
	Serial.print("Flashing");
	while (fwFile.available())
	{
		data_read = fwFile.read(buff, UPDATE_BUFF_SIZE);
		Serial.print(".");
		
		if (Update.write(buff, data_read) != data_read)
		{
			Serial.print("\nError: ");
			Update.printError(Serial);
			fwFile.close();
			deleteSourceFile();
			return;
		}

	}
	
	fwFile.close();
	
	if (Update.end(true))
	{
		Serial.println("\nSuccess!");
		
		deleteSourceFile();
		
		Serial.println("\n\nREBOOTING NOW!");
		delay(250);
		ESP.restart();
	}
	else
	{
		Serial.print("\nError: ");
		Update.printError(Serial);
		
		deleteSourceFile();
	}

}

void deleteSourceFile()
{
	Serial.print("Removing update file: ");
	if(SD.remove(update_filename))
	{
		Serial.println("OK");
	}
	else
	{
		Serial.println("Error, going to sleep to avoid flash loop");
		ESP.deepSleep(0);
	}
}

void loop()
{
	
}

