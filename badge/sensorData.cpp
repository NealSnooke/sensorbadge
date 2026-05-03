
#include "sensorData.h"

sensorInfo sensorReadings[NSENSORS+1];
boolean noReadings = true;

void initReadings(){
  for (int i=0; i<=NSENSORS; i++){
    sensorReadings[i].recent = i;
    sensorReadings[i].temp = 0.0;
    sensorReadings[i].moist = 0.0;
  }
}

void clearHistorySequence(){
  for (int i=0; i<=NSENSORS; i++){
    sensorReadings[i].recent = 0;
  }
}

/**
 * increase the recent value to push a new reading into the 'list'
 */
void bumpReadings(){
  for (int i=0; i<=NSENSORS; i++){
    if (sensorReadings[i].recent != 0){
      sensorReadings[i].recent = sensorReadings[i].recent+1;
    }
  }
}

void printSensors(){
  for (int i=1; i<=NSENSORS; i++){
    
    Serial.print(i);
    Serial.print(" recent:");
    Serial.print(sensorReadings[i].recent);
    Serial.print(" Temp:");
    Serial.print(sensorReadings[i].temp);
    Serial.print(" Moist:");
    Serial.print(sensorReadings[i].moist);

    Serial.println();
  }
}