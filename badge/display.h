#ifndef _DISP_H_
#define _DISP_H_

#include <Arduino.h>
#include <TFT_Touch.h>
#include <TFT_eSPI.h> // Hardware-specific library

#define FlashFS LittleFS
#define AA_FONT_SMALL "NotoSansBold15"
#define AA_FONT_LARGE "NotoSansBold36"
#define AA_FONT_16 "crazyh16"
#define AA_FONT_20 "crazyh20"
#define AA_FONT_25 "crazyh25"
#define AA_FONT_35 "crazyh35"
#define AA_FONT_40 "crazyh40"

#define BB_FONT_12 "verdana12"
#define BB_FONT_14 "verdana14"
#define BB_FONT_16 "verdana16"
#define BB_FONT_18 "verdana18"
#define BB_FONT_20 "verdana20"
#define BB_FONT_24 "verdana24"
#define BB_FONT_36 "verdana36"
#define BB_FONT_48 "verdana48"

// These are the pins used to interface between the 2046 touch controller and Arduino Pro
#define DOUT 39  /* Data out pin (T_DO) of touch screen */
#define DIN  32  /* Data in pin (T_DIN) of touch screen */
#define DCS  33  /* Chip select pin (T_CS) of touch screen */
#define DCLK 25  /* Clock pin (T_CLK) of touch screen */

// number of buttons per row
#define NBPR 10
#define BUTTON_GAP 4;
#define SELECTED_BUTTON_COLOUR TFT_DARKGREY

extern TFT_eSPI tft;
extern TFT_Touch touch;

extern int16_t mxpos;
extern int16_t mypos;
extern unsigned int colour;
extern int currentSensor;
extern bool penUsed;

void showSensor(int reading);
void showBackground(bool clear);
void showButtons(bool clearSelected);
void showButton(int i, bool clearSelected);
void showMessage(const char* message, uint8_t size);
void resetMessage();
bool clickButton(int x, int y);
void updateActiveSensorCount();
//void clearSensorScreen();
//void clearSensorScreenTop();

uint32_t getTFT_Colour(int gradientIndex, double val); // fetch a tftcolour from a gradient specification

#endif

