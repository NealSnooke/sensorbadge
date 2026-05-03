# Peaty things 
**Pwllperian soil sensor CYD badge project**

This project implements a badge that subscribes to an MQTT server that displays soil sensor measurements from Pwllperion. The idea is to display the soil temperature and moisture valued as they are published from each sensor. 

The badge also provides sound notifications (if a micro speaker is connected to the CYD) and allow the user to draw their name on the badge or draw little pictures!

## Source

The project is a C++ program based on the Arduino libraries. The projcet and thus the folder is called "badge".

The project is based on the CYD (Chealp Yellow Display) boards available from Ali Express,  Ebay [Amazon](https://eur02.safelinks.protection.outlook.com/?url=https%3A%2F%2Fwww.amazon.co.uk%2FDiymore-Display-Bluetooth-Development-Resolution%2Fdp%2FB0DXFBKKQB%2F&data=05%7C02%7C%7Cb4aa6225b6744a9720bb08dea41eefc7%7Cd47b090e3f5a4ca084d09f89d269f175%7C0%7C0%7C639128649929235883%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=8G0Fz61cr17cIjGYYxxoQ2ka0NQbVoSIKLXBAC8XhGk%3D&reserved=0>)
etc.


# Project E32R28T Board -  Notes 

Board specifications for reference:

* Chip type:          ESP32-D0WD-V3 (revision v3.1)
* Features:           Wi-Fi, BT, Dual Core + LP Core,  240MHz, Vref calibration in eFuse, Coding Scheme None
* Crystal frequency:  40MHz
* MAC:                a4:f0:0f:68:28:ac

ESP32-32E

* ESP32-D0WD-V3,Xtensa dual-core 32-bit LX6 microprocessor
* Have Touch Sreen: E32R28T
* Driver chip ILI9341V, 240x320(pixels) XPT2046
* Touch screen XPT2046 controller

Initial datasheet:
<http://8.217.75.21/Industrial/Multilingual/CBAA0046-032_UK.pdf>

Which links to:

User manual website (sort of)
<https://www.lcdwiki.com/2.8inch_ESP32-32E_Display#ESP32_Parameters>

The most useful info is then the "Demo Instructions" pdf.

<https://www.lcdwiki.com/res/E32R28T/2.8inch_E32R28T_E32N28T_ESP32-32E_Demo_Instructions.pdf>

because it has a sensible table of pin allocations with names that match what most libraries use MOSI/MISO etc for an SPI bus... From that it is possible to grab Arduino libraries also listed in that pdf:

* `TFT_eSPI` for the display chip driver
* `TFT_Touch` for the touch screen

The display driver provides functions at the level of drawing text, boxes, lines fill areas etc. It needs a few changes to the headerfiles in the library to match the hardware (see section below)

The Touch library is easier and has the required pins as defines in the user code. 

The Arduino 'library search' really didn't seem to want to find the libraries. It did eventually find the screen driver one after trying various searches. I never got it to find the touch library and got it from GitHub. 

BTW. the E32N28T version does not have the touch screen capability.

## Ardunio:

I used Board: ESP32/ESP32 Dev Module and it seems fine.

As usual for all ESP32's I had to change Arduino environment programming speed to 115200 from the default 921600 that never works on my Macbook Pro to solve "A fatal error occurred: The chip stopped responding." when uploading. 

# LCD display

library git: <https://github.com/Bodmer/TFT_eSPI>

useful video <https://www.youtube.com/watch?v=oQrVjc9DhZk>

Edit TFT\_eSPI library to set correct pins specified in the 
 
`2.8inch_E32R28T_E32N28T_ESP32-32E_Demo_Instructions.pdf`

file in Arduino libraries:

`/Users/nealsnooke/Documents/Arduino/libraries/TFT_eSPI/User_Setups/Setup42_ILI9341_ESP32.h`

```#define TFT_MISO 12  // (leave TFT SDO disconnected if other SPI devices share MISO)
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
```

must turn on Backlight for display to function

```
// turn on backlight
pinMode(21, OUTPUT);
digitalWrite(21, HIGH);
```

edit

`/Users/nealsnooke/Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
`
 
to replace the default user setup with the one above

```
//#include <User_Setup.h> // Default setup is root library folder

#include <User_Setups/Setup42_ILI9341_ESP32.h> // Setup file for ESP32 and SPI ILI9341 240x320
```
# Touch driver

put the project folder from git into the ardunio library folder (arduino didn't find it using the library tool for me)

<https://github.com/Bodmer/TFT_Touch>

`#include <TFT_Touch.h>`

This library simplgy needs the pins setting in the user program 

```
// These are the pins used to interface between the 2046 touch controller and Arduino Pro
#define DOUT 39  /* Data out pin (T_DO) of touch screen */
#define DIN  32  /* Data in pin (T_DIN) of touch screen */
#define DCS  33  /* Chip select pin (T_CS) of touch screen */
#define DCLK 25  /* Clock pin (T_CLK) of touch screen */
```

# smooth fonts


https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/#installing-MAC

For smooth fonts go to this site and create a font from the ttf font file (vlw file)

<https://vlw-font-creator.m5stack.com>

Then put that file in the data folder of the arduino projcet

Then upload to ESP using LittleFS file syste to store fonts 
use commmand-shift-p and use file uploader tool:

<https://ttfonts.net/font/3862_Bertram.htm>
