
#include "display.h"
#include "animation.h"

int state = 0; //used for the screen clear feature

int statetone = 0; //used for the sound feature
bool soundoff = true; //default on start up

int clrhtop = 35;
int clrhtopdist = 90;
int16_t swipeColour = TFT_GREENYELLOW;

void setClrTop(){
  if (state != 0) return; // already clearing

  clrhtop = 35;
  clrhtopdist = 90;
  state=1;
}

void setClrAll(){
  //if (state != 0) return; // already clearing
  clrhtop = 35;
  clrhtopdist = 240-33-32*2;
  state=1;
}
//tft.fillRect(0, 35, 320, 240-33-32*2, TFT_BLACK); // clear the whole area

/*
 * next step in an animation
 */
void animation(){
  //Serial.print("animate");
  //Serial.println(state);
  
  if (state > 0) {

    if (state <= HWIDTH){
      // clear horizontally
      tft.drawFastVLine(state-4, clrhtop, clrhtopdist, TFT_BLACK);
      tft.drawFastVLine(WIDTH-state+4, clrhtop, clrhtopdist, TFT_BLACK);
    }

    if (state <= HWIDTH) {
        // draw vertical yellow lines
        tft.drawFastVLine(state, clrhtop, clrhtopdist, swipeColour);
        tft.drawFastVLine(WIDTH-state, clrhtop, clrhtopdist, swipeColour);
        state++;
    } else {
      if (state < HWIDTH+clrhtopdist) {
      // clear vertically
        tft.drawFastHLine(HWIDTH-4, clrhtop-1+(state-HWIDTH), 8, TFT_BLACK);
        tft.drawFastHLine(HWIDTH-4, clrhtop+clrhtopdist-(state-HWIDTH), 8, TFT_BLACK);
        state++;
        
      } else {
        state = 0;
        showSensor(currentSensor); // display it
      }
    }
  }
}

/*
 *
 */
void badgeTone(){
  if (soundoff) return;

  // set statetone to iniial frequency value
  if (statetone > 0) {
    tone(26, statetone, 20);
    statetone += 150;

    if (statetone>2000)
      statetone = 0; //turn off
  }

  // set eg. to -7000 will result in tones down to 300hz
  if (statetone < 0) {
    tone(26, -statetone, 10);
    statetone += 50;

    if (statetone > -300)
      statetone = 0; //turn off
  }

}