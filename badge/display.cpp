#include "display.h"
#include "sensorData.h"
#include "animation.h"
#include <LittleFS.h>

/* Create an instance of the touch screen library */
TFT_Touch touch = TFT_Touch(DCS, DCLK, DIN, DOUT);
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

struct tm timeinfo; // time struct
char locTime[13]; // the time string

int currentSensor = 0; //the sensor is being (will be) shown 

bool penUsed = false; // one time flag

byte selectedButton = 0; // filled button
int activeSensors = 1; // number of sensors active so far, used for colouring buttons
unsigned int colour = TFT_GOLD; // 5, 6, 5 bits for RGB randomly changing

int NeoColours[5][2][3] = {
  {{0, 0, 192},{192, 0, 0}}, // blue, red (soil temp)
  {{0, 0, 20}, {0, 200, 200}}, // dim blue -> cyan blue (soil moisture)
  {{25, 25, 0}, {0, 255, 0}}, //yellow -> green (C02) 
  {{55, 12, 0}, {255 , 0, 0}}, // orange -> red (methane)
  {{0, 16, 0},{255, 255, 0}}, // HISTORY_COL button borders 
};

#define TEMP_COL 0
#define HISTORY_COL 4

/**
 * display whole sensor reading
 
void clearSensorScreen(){
  //tft.fillRect(0, 35, 320, 240-33-32*2, TFT_BLACK); // clear the whole area
  setClrAll();
}

void clearSensorScreenTop(){
  //tft.fillRect(0, 35, 320, 90, TFT_BLACK); // clear the top area
  setClrTop();
}*/

void drawSpeakerIcon(){


  uint16_t col = TFT_DARKGREY;

  tft.drawWideLine(300, 2, 318, 21, 3, TFT_BLACK, TFT_BLACK); //clear red line
  
  tft.drawWideLine(300, 3,  300, 23, 3, col, TFT_BLACK); //vertical

  tft.drawWideLine(300, 3,  312, 8,  3, col, TFT_BLACK); //top
  tft.drawWideLine(300, 23, 312, 15, 3, col, TFT_BLACK); //bottom

  tft.drawWideLine(312, 5,  312, 19, 3, col, TFT_BLACK); //right

  if (soundoff) {
    //tft.drawWideLine(300, 8, 315, 15, 3, TFT_RED, TFT_BLACK);
    tft.drawWideLine(300, 2, 318, 22, 3, TFT_RED, TFT_BLACK);
  }

}

void drawClearIcon(){
    uint16_t col = TFT_DARKGREY;
    //x,y,r1,r2,w,h
    tft.drawSmoothRoundRect(3, 3, 2, 3, 20, 20, col , TFT_BLACK);
    //x, y, w, h, radius,  color,  bg_color
    //tft.fillSmoothRoundRect(11, 3, 10, 20, 2, col , TFT_BLACK);
}

/**
 * display sensor reading (reading is sensor index number)
 */
void showSensor(int reading){
      Serial.print("showSensor ");Serial.println(reading);

      tft.setTextDatum(CL_DATUM); //left center datum
      tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK); // foreground, background

      int precision = 2;  // Number of digits after decimal point
      int font = 4;       // font number 2

      int16_t xpos = 75;
      int16_t ypos = 58;

      //tft.fillRect(0, 35, 320, 240-35-32*2-1, TFT_BLACK); // clear the whole area
      //tft.fillRect(0, 35, 320, 95, TFT_BLACK); // clear the top area
      //clearSensorScreenTop();

      
      if (reading == -1){
        tft.loadFont(AA_FONT_25, LittleFS); // Load another different font
        tft.setTextDatum(CC_DATUM);
        tft.drawString("No sensor messages", HWIDTH, ypos, font);
        tft.unloadFont(); // Remove the font to recover memory used
        return;
      }

      tft.loadFont(AA_FONT_35, LittleFS); // Load another different font
      xpos += tft.drawString("Sensor  ", xpos, ypos);
      xpos += tft.drawNumber(reading, xpos, ypos);
      //xpos += tft.drawString("Sensor  ", xpos, ypos, font);
      //xpos += tft.drawNumber(reading, xpos, ypos, 6);

      tft.unloadFont(); // Remove the font to recover memory used
      tft.loadFont(AA_FONT_20, LittleFS); // Load another different font
      //tft.loadFont(BB_FONT_18, LittleFS); // Load another different font
      xpos = 10;
      //ypos = 110;
      ypos = 100;
      tft.setTextColor(TFT_WHITE, TFT_BLACK); // foreground, background

      if (sensorReadings[reading].recent != 0) { //we have a reading

        //xpos += tft.drawString("Temp  ", xpos, ypos, font); 
        //xpos += tft.drawFloat(sensorReadings[reading].temp, precision, xpos, ypos, font);
        xpos += tft.drawString("Temp  ", xpos, ypos); 
        xpos += tft.drawFloat(sensorReadings[reading].temp, precision, xpos, ypos);

        tft.setTextDatum(CR_DATUM); // datum  
        xpos = 310;
        //xpos -= tft.drawFloat(sensorReadings[reading].moist, precision, xpos, ypos, font);
        //xpos -= tft.drawString("Moist  ", xpos, ypos, font); 
        xpos -= tft.drawFloat(sensorReadings[reading].moist, precision, xpos, ypos);
        xpos -= tft.drawString("Moist  ", xpos, ypos); 
      } else {
        xpos = 160;
        tft.setTextDatum(CC_DATUM); // datum  
        //xpos -= tft.drawString("No data available", xpos, ypos, font);
        xpos -= tft.drawString("No data available", xpos, ypos);
      }

      tft.unloadFont(); // Remove the font to recover memory used
}

int16_t mxpos = 0;
int16_t mypos = 0;

/**
 *
 */
void resetMessage(){
  tft.fillRect(0, 35, 320, 150, TFT_BLACK); // clear the area
  mxpos = 160;
  mypos = 40;
};

/**
 * display a message
 */
void showMessage(const char* message, uint8_t mfont){
      tft.setTextDatum(TC_DATUM ); //left center datum
      
      if (mfont == 4) {
        tft.loadFont(BB_FONT_20, LittleFS); // Load another different font
        tft.setTextColor(TFT_GREENYELLOW, TFT_BLACK); // foreground, background
      } else {
        tft.loadFont(BB_FONT_16, LittleFS); // Load another different font
        tft.setTextColor(TFT_WHITE, TFT_BLACK); // foreground, background
      }

      //tft.drawString(message, mxpos, mypos, mfont); 
      tft.drawString(message, mxpos, mypos); 
      mypos += tft.fontHeight(mfont)+7;
      tft.unloadFont(); // Remove the font to recover memory used
}

/**
 *
 */
void showBackground(bool clear){
  tft.setTextDatum(TC_DATUM ); //top center datum
  tft.setTextColor(TFT_ORANGE, TFT_BLACK); // foreground, background

  if (clear) tft.fillRect(0, 0, 320, 35, TFT_BLACK); // clear the area

  int font = 2;       // font number 2
  tft.drawString("Pwllpeiran LoRaWan Soil Sensor Network", 160, 2, font);

  if (getLocalTime(&timeinfo)){
    //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    //tft.setTextColor( TFT_GOLD, TFT_BLACK); // foreground, background
    tft.setTextColor(colour, TFT_BLACK); // foreground, background

    int16_t xpos = 160;
    int16_t ypos = 20;
    sprintf(locTime, "%02d : %02d : %02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    /*tft.fillRect(120, 20, 90, 16, TFT_BLACK); 
    tft.loadFont(BB_FONT_14, LittleFS); // Load another different font
    xpos += tft.drawString(locTime, xpos, ypos); 
    tft.unloadFont(); // Remove the font to recover memory used
    */
    xpos += tft.drawString(locTime, xpos, ypos, font); 
  }

  drawSpeakerIcon();
  drawClearIcon();
}

/*
 *
 */
void updateActiveSensorCount(){
  activeSensors = 0; 

  for (int i=1; i<=NSENSORS; i++){
    if (sensorReadings[i].recent != 0) activeSensors++;
  }
}

/**
 * draw sensor button boxes
 */
void showButtons(bool clearSelected){
  tft.setTextDatum(TC_DATUM ); //top center datum
  tft.setTextColor(TFT_ORANGE, TFT_BLACK); // foreground, background

  updateActiveSensorCount();

  for (int i=1; i<=NSENSORS; i++){
    showButton(i, clearSelected);
  }

}

/**
 * draw a button
 * note buttons 0->n maps to sensors 1->n+1, selectet button is the sensor number, 0 if no selection
 */ 
void showButton(int i, bool clearSelected){
  int font = 2;       // font number 2

  int col = (i-1)%NBPR;
    int row = (i-1)/NBPR;

    int btnsze = 320/NBPR;
    int btnwdh = btnsze - BUTTON_GAP;

    // center of button
    int xpos = col * btnsze + btnsze/2;
    int ypos = row * btnsze + btnsze/2;
   
    //Serial.print(xpos); Serial.print(","); Serial.print(ypos); 
    int32_t tftcol = getTFT_Colour(HISTORY_COL, 1-(double)sensorReadings[i].recent/activeSensors);
    if (activeSensors==0) tftcol = TFT_BLACK;

    if (sensorReadings[i].recent==0) tftcol = TFT_BLACK; //sensors that have not responded ever
    if (sensorReadings[i].recent==1) tftcol = TFT_WHITE; // most recent reading
    //Serial.println(tftcol);

    if (clearSelected) {
      tft.fillRect(xpos-btnwdh/2, 240-ypos-btnwdh/2, btnwdh, btnwdh, TFT_BLACK);
      selectedButton = 0;
    }
    
    //tft.drawRect(xpos-btnwdh/2, 240-ypos-btnwdh/2, btnwdh, btnwdh, tftcol);
    //tft.drawRoundRect(xpos-btnwdh/2, 240-ypos-btnwdh/2, btnwdh, btnwdh, 4, tftcol);
    tft.drawSmoothRoundRect(xpos-btnwdh/2, 240-ypos-btnwdh/2, 6, 5, btnwdh, btnwdh, tftcol, TFT_BLACK);

    if (i == selectedButton){
      //tft.fillRect(xpos-btnwdh/2+2, 240-ypos-btnwdh/2+2, btnwdh-4, btnwdh-4, SELECTED_BUTTON_COLOUR);
      tft.fillSmoothRoundRect(xpos-btnwdh/2+2, 240-ypos-btnwdh/2+2, btnwdh-3, btnwdh-3, 4,  SELECTED_BUTTON_COLOUR, tftcol);
      tft.setTextColor(TFT_ORANGE, SELECTED_BUTTON_COLOUR); // foreground, background
    } else {
      tft.setTextColor(TFT_ORANGE, TFT_BLACK); // foreground, background
    }
    

    tft.setTextDatum(CC_DATUM); //center center datum
    tft.drawNumber(i, xpos, 240-ypos, font); 
  }

/**
 * detectes if a button was pressed and triggers display of the associated recent sensor data
 */
bool clickButton(int x, int y){
  // detect corners

  // top right corner
  if (x>220 && y<20) {

    soundoff = !soundoff;
    if (soundoff) 
      tone(26, 100, 100);
    else 
      tone(26, 300, 100);

    drawSpeakerIcon();
    delay(100);
    Serial.print("sound off");Serial.println(soundoff);

  }

  // top left corner
  if (x<20 && y<20) {
    setClrAll();
  }

  int btnsize = 320/NBPR;
  y = 240 - y;

  if (y<0|| y>btnsize*2-6) return false; // above the buttons 2 is to allow to draw close to the buttons

  int col = x/btnsize;
  int row = y/btnsize;

  Serial.println(row);
  Serial.println(col);

  showButton(selectedButton, true); //clear selected button
  selectedButton = col+row*NBPR+1;
  showButton(selectedButton, false); // button mubers 1 less than sensor numbers

  if (currentSensor == selectedButton ) return true; // done redisplay if same button

  // clear all or part of the area - all if the sensor has no readings

  Serial.print("Set current sensor "); Serial.println(selectedButton);
  
  if (sensorReadings[selectedButton].recent == 0) {
    currentSensor = selectedButton; // comment out for no data yet instead of sensor number and no data message
    setClrTop();
    statetone = -600; // downbeat sequence
  } else {
    currentSensor = selectedButton; // set new sensor number 
    setClrTop();// trigger screen clear and redisplay 
  }

  showBackground(true);
  return true;
}

/**
 * get a colour in TFT condensed rgb format that lies between two colours
 * val between 0 and 1 for full range
 */
uint32_t getTFT_Colour(int gradientIndex, double val){

  //Serial.print("colour:");
  //Serial.println(val);

  if (val >= 1) val = 0.999;
  if (val < 0) val = 0;
  
  byte red = (byte)(val*(NeoColours[gradientIndex][1][0]-NeoColours[gradientIndex][0][0]) + NeoColours[gradientIndex][0][0]);
  byte green = (byte)(val*(NeoColours[gradientIndex][1][1]-NeoColours[gradientIndex][0][1]) + NeoColours[gradientIndex][0][1]); 
  byte blue = (byte)(val*(NeoColours[gradientIndex][1][2]-NeoColours[gradientIndex][0][2]) + NeoColours[gradientIndex][0][2]);

  //Serial.println(red);
  //Serial.println(green);
  //Serial.println(blue);

  //return (green & 0b11111100) << 3;
  //return (red & 0b11111000) << 8;
  //return (blue >> 3);
  //return (red & 0b11111000) << 11 | (green & 0b11111100) << 5 | (blue >> 3);
  return (red & 0b11111000) << 8 | (green & 0b11111100) << 3 | (blue >> 3);
}