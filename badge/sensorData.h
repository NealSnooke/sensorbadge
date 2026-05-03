#ifndef _SDATA_H_
#define _SDATA_H_

#include <Arduino.h>

#define NSENSORS 20

struct sensorInfo {
  int recent; // most recent is 1 least is highest number
  double moist;
  double temp;
};

extern sensorInfo sensorReadings[NSENSORS+1];

void initReadings();
void clearHistorySequence();
void printSensors();
void bumpReadings();

#endif

