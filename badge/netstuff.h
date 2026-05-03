#ifndef _NSTUF_H_
#define _NSTUF_H_

#include <Arduino.h>

//void setup_wifi(); 
bool setup_wifi();
void connectMQTT();
void callback(char* topic, unsigned char* payload, unsigned int length);
void reconnect();
void net_loop();

extern int messagesRecd;

#endif

