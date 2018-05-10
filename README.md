Example showing how to update an ESP8266 from a file on an attached SDCard, useful for dataloggers.

If a file named "update.bin" is found it will perform a few checks and then quickly flash it to the ESP

Things to note:
- Just like OTA the ESP requires enough free memory to store both the current sketch and the new one
- If it fails you probably have set the wrong flash size in the arduino IDE, use the CheckFlashConfig sketch included with ESP8266 arduino
- The update file will be deleted after flashing unless it is detected to be bad before flashing
