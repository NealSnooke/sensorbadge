/*

320x240 LCD touch display

Connected to ESP32 on /dev/cu.usbserial-110:
Chip type:          ESP32-D0WD-V3 (revision v3.1)
Features:           Wi-Fi, BT, Dual Core + LP Core, 240MHz, Vref calibration in eFuse, Coding Scheme None
Crystal frequency:  40MHz
MAC:                a4:f0:0f:68:28:ac

ESP32-32E
ESP32-D0WD-V3,Xtensa dual-core 32-bit LX6 microprocessor
Have Touch Sreen: E32R28T/Users/nealsnooke/RESEARCH/PROTOTYPES/touch screen ESP32
Driver chip ILI9341V, 240x320(pixels) XPT2046
Had to change programming speed to 115200 to solve "A fatal error occurred: The chip stopped responding." when uploading

*/
// https://www.youtube.com/watch?v=oQrVjc9DhZk
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
// Call up touch screen library
#include <TFT_Touch.h>

// network stuff for MQTT connection
#include "netstuff.h"
#include <WiFi.h> // to fetch mac address

//sensor data
#include "sensorData.h"

// touch screen display
#include "display.h"

#include "animation.h"

// Font files are stored in Flash FS
#include <FS.h>
#include <LittleFS.h>

//10, 5 is faster
#define LOOPSPEED 5

byte red = 31;
byte green = 0;
byte blue = 0;

bool screenOff = false;

const char* ntpServer = "pool.ntp.org";

void setup() {

  Serial.begin(115200); 
  delay(500);         
  // put your setup code here, to run once:
  Serial.println("Hello");

   // fonts
  if (!LittleFS.begin()) {
    Serial.println("Flash FS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\n\Flash FS available!");

  bool font_missing = false;
  if (LittleFS.exists("/crazyh16.vlw") == false) font_missing = true;
  if (LittleFS.exists("/crazyh20.vlw") == false) font_missing = true;
  if (LittleFS.exists("/crazyh25.vlw") == false) font_missing = true;
  if (LittleFS.exists("/crazyh35.vlw") == false) font_missing = true;
  if (LittleFS.exists("/crazyh40.vlw") == false) font_missing = true;
  if (LittleFS.exists("/verdana18.vlw") == false) font_missing = true;
  if (LittleFS.exists("/verdana20.vlw") == false) font_missing = true;

  if (font_missing)
  {
    Serial.println("\nFont missing in Flash FS, did you upload it?");
    while(1) yield();
  }
  else Serial.println("\nFonts found OK.");

  // turn on backlight
  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  tft.init();
  tft.setRotation(0);

  //This is the calibration line produced by the TFT_Touch_Calibrate_v2 sketch
  touch.setCal(481, 3395, 755, 3487, 320, 240, 1);
  // Set the TFT and touch screen to landscape orientation
  tft.setRotation(1);
  touch.setRotation(1);

  tft.fillScreen(TFT_BLACK);
  showBackground(false);

  resetMessage();
  showMessage("Connecting WiFi...", 2);

  initReadings();
  showButtons(false);

  // Get MAC address of the WiFi station interface
  //https://forum.arduino.cc/t/issue-with-wifi-macaddress-and-wifi-softapmacaddress-on-esp32-with-arduino-core-v3-2-0/1374071
  WiFi.mode(WIFI_MODE_APSTA);
  Serial.print("MAC address: "); 
  Serial.println(Network.macAddress());

  if (!setup_wifi()) {
    showMessage("  ", 2); // add space
    showMessage("Rebooting...", 4);
    delay(4000);
    ESP.restart();
  }

  // get the time from the internet
  configTime(3600*0, 3600, ntpServer); // Set time: 3600*1 for GMT+1, 3600 for DST
  
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  showBackground(false); // add time

  mypos += 10;
  connectMQTT();

  // speaker output on
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  currentSensor = -1;
}

/**
 *
 */
void loop() {
  unsigned long lastTouch = millis();

  int X_Coord;
  int Y_Coord;
  int multiplier = 10/LOOPSPEED;

  // put your main code here, to run repeatedly:
  int i=1;

  while (true) {

    net_loop();

    if (i%(multiplier*2000+1) == 0) { // update colour
      //change drawing colour
      red = random(255);
      green = random(255);
      blue = random(255); 
      colour = red << 11 | green << 5 | blue;
      //showBackground(false); // update time each second
    }

    if (i%(multiplier*100) == 0) {
      showBackground(false); // update time each second
    }

    if (i%(multiplier*20+1) == 0 && messagesRecd==0) {
      // make the buttons fade out at start up by increaing the recent-ness of buttons
      for (int s=1; s<=NSENSORS; s++){
        if (sensorReadings[s].recent > 0) sensorReadings[s].recent = sensorReadings[s].recent+1;
        if (sensorReadings[s].recent > NSENSORS*2) sensorReadings[s].recent = 0;
      }

      showButtons(false);
    }

    // service the animation elements
    if (i%10) {
      animation();
    }

    // service the sound
    if (i%5) {
      badgeTone();
    }

    if (millis()-lastTouch > 3*60*1000){ // screen saver 3 mins
      if (!screenOff ) {
        Serial.println("Screen off");
        digitalWrite(21, LOW); // backlight/screen off
        screenOff = true;
      }
    }
      

    if (touch.Pressed()) // Note this function updates coordinates stored within library variables
      {
        lastTouch = millis();
        if (screenOff) {
          Serial.println("Screen on");
          digitalWrite(21, HIGH); // backlight/screen on
          screenOff = false;
          showButtons(false);
        }

        if (!penUsed && messagesRecd==0) {
          //currentSensor = -1;
          setClrAll();// trigger screen clear and redisplay 
          penUsed = true;
        } else {
          // Read the current X and Y axis as co-ordinates at the last touch time
          // The values were captured when Pressed() was called!
          X_Coord = touch.X();
          Y_Coord = touch.Y();

          if (!clickButton(touch.X(), touch.Y())){ // false if no button pressed

            //Serial.print(X_Coord); Serial.print(","); Serial.println(Y_Coord);
            if ((Y_Coord > 35+90) && (Y_Coord < (240-(320/NBPR)*2)-3)) // restrict to top arrea and above buttons from bottom
              tft.fillCircle(X_Coord, Y_Coord, 2, colour);
          }
        }
      }

    delay(LOOPSPEED);

    i++;
  }

}
