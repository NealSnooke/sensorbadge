
#include "netstuff.h"
#include <WiFi.h>
#include <PubSubClient.h>

#include "sensorData.h" // so we can store recent sensor values
#include "display.h" // so we can display values on the badge
#include "animation.h"

// debug to check the indices of files queued to play (see audiostuff.h for mapping)
#define DEBUG_PHRASE_LIST
// number of times to try each wi network before moving on
#define WIFI_CONNECT_TRIES 10

// output the remote data in detail
#define DEBUG_REMOTE_DATA

/** *
const char* ssid = "FRITZ!Box 7530 GE";
const char* password = "67086333856636297221";
*/

/** */
const char* ssid = "moto g(8) power_4545";
const char* password = "8y5pwqtfvh5rf52";
/* */
const char* mqtt_server = "eu1.cloud.thethings.network";

extern bool doneDataPoint;

int messagesRecd = 0;

// Update these with values suitable for your network.
IPAddress local_IP(192,168,250,110);
IPAddress gateway(192,168,250,254);
IPAddress subnet(255,255,255,0);
IPAddress primaryDNS(192,168,250,254);
IPAddress secondaryDNS(192,168,250,254);

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(80)
char msg[MSG_BUFFER_SIZE];

/**
 * @ return true if connected, false otherwise
 */
bool setup_wifi() {
  showMessage(ssid, 4);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Trying to connect to ");
  Serial.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  static bool toggl = false;
  
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < WIFI_CONNECT_TRIES) 
  {
    tries++;

    delay(500);
    Serial.print(".");
    toggl = !toggl;

      if (tries > 20) { // reboot if we are not getting any success... just in case
        Serial.println("Failed to connect to WiFi - restarting");
            ESP.restart();
      }
  }

  if (WiFi.status() != WL_CONNECTED) {
    showMessage("Failed - check network", 2);
    return false;
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  static char dumstr[50];
  sprintf( dumstr, "%s", WiFi.localIP());

  showMessage("Connected", 2);
/*
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // moved here into setup from Loop so that conection doesn't drop
  if (!client.connected()) reconnect();
*/
  return true; 
}


/**
 *
 */
void connectMQTT(){
  showMessage("Connecting MQTT server...", 2);
  showMessage(mqtt_server, 4);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // moved here into setup from Loop so that conection doesn't drop
  if (!client.connected()) reconnect();
  showMessage("Connected", 2);
}

/**
 *
 */
void callback(char* topic,  unsigned char* payload, unsigned int length) {
  if (messagesRecd==0) {
    //clearSensorScreen();
    clearHistorySequence(); // clear the buttons
  }

  

  String fred = "";
  int lightlev;
  int subval;
  double dval;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    fred.concat((char)payload[i]);
  }

  #ifdef DEBUG_REMOTE_DATA
  Serial.print(fred);
  Serial.println();
  Serial.print("Length: ");
  Serial.print(length);
  Serial.println();
  #endif

  //if(sscanf(fred,"\"light_intensity\":%d",&lightlev)==1)
  //if(fred.indexOf("\"snr\":") != 0)
  //int lightindex = fred.indexOf("\"light_intensity\":");
  int moist_index = fred.indexOf("\"adj_watermark1\":");
  int temp_index = fred.indexOf("\"temperature1\":");
  int probeno_index = fred.indexOf("ff-");

  //char *myPtr = strstr((const char *)payload, "abc");
   
  if (probeno_index != 0){
    Serial.print("found probeno_index at ");
    Serial.print(probeno_index);

    String subs = fred.substring(probeno_index+3, probeno_index+5);
    Serial.print(" substring is \"");
    Serial.print(subs);
    Serial.print("\" value is ");
    int subvalprobe = subs.toInt();
    Serial.println(subvalprobe);

    //clearHistorySequence();

    bumpReadings(); // push reading down list
    sensorReadings[subvalprobe].recent = 1;
    sensorReadings[subvalprobe].temp = 0.0; //init
    sensorReadings[subvalprobe].moist = 0.0; //init

    //add_to_phrase(Fsilence);
    //add_to_phrase(Fprobe);
    //add_to_phrase(subval);
  
    if (temp_index != 0) {
      Serial.print("found temp_index at ");
      Serial.print(temp_index);

      String subs = fred.substring(temp_index+15,temp_index+25);
      Serial.print(" substring is \"");
      Serial.print(subs);
      Serial.print("\" value is ");
      dval = subs.toDouble();
      Serial.println(subs.toDouble());

      sensorReadings[subvalprobe].temp = dval;

      //add_to_phrase(Ftemperature);
      //add_to_phrase(Fmeasured);
      //playDouble(dval);
      //add_to_phrase(Fdegrees);
    }

    if (moist_index != 0) {
      Serial.print("found moist_index at ");
      Serial.print(moist_index);

      String subs = fred.substring(moist_index+17,moist_index+27);
      Serial.print(" substring is \"");
      Serial.print(subs);
      Serial.print("\" value is ");
      dval = subs.toDouble();
      Serial.println(subs.toDouble());

      sensorReadings[subvalprobe].moist = dval;

      //add_to_phrase(Fsilence);
      //add_to_phrase(Fmoisture);
      //add_to_phrase(Fmeasured);
      //playDouble(dval);
      //add_to_phrase(Fpercent); // I don't think it is a percent?
    }
  
    //showSensor(subvalprobe);
    Serial.println("New sensor reading "); Serial.println(subvalprobe);

    currentSensor = subvalprobe; // set sensor number 
    if (messagesRecd==0 && !penUsed) 
      setClrAll();// trigger screen clear and redisplay 
    else 
      setClrTop();// trigger screen clear and redisplay 
  
    messagesRecd++;
    statetone = 300; //start at 300hz

    showBackground(true);

    showButtons(true);

    printSensors();
  }
}

/**
 *
 */
void reconnect() {
  // Loop until we're reconnected
  //delay(1000);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
   // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
  //String clientId = "mqtt-access";
    // Attempt to connect
   if (client.connect(clientId.c_str(),"antur-soil-sensors@ttn","NNSXS.NHXGTLVW7PAIG2D5MFGRWV3E67MXQI6N2X3BSPA.APLLS6W4APXBZGOSYCY7YFCYXZY66EZUF733ND3W5VACYZPJ67AA" )) {
      Serial.println("connected");
      Serial.println("connected");
      // Once connected, publish an announcement...
   //   client.publish("outTopic", "hello world");
      // ... and resubscribe
     // client.subscribe("v3/mqtt-access/devices/eui-647fda0000009543/#");
     client.setBufferSize(16384);
  //   client.subscribe("v3/mqtt-access/devices/eui-647fda0000009543/up");
  //   client.subscribe("v3/soil-sensors/devices/+/up");
     client.subscribe("v3/antur-soil-sensors@ttn/devices/+/up");
//     client.subscribe("#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);

    }
  }
}

/**
 *
 */
void net_loop()
{
      if (!client.connected()) {
        Serial.println("Disconnected");
        delay(1000);
        reconnect();
      }

      client.loop();
}


