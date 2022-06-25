#include<SoftwareSerial.h>
#include<ArduinoJson.h>

char c;
String dataIn;
int8_t IndexofA,IndexofB;
String data1,data2;

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>


//----------- Enter you Wi-Fi Details---------//
char ssid[] = "MMGHEEWALE HOME";    //your network SSID (name)
char pass[] = "389cs15027";         //your network password
//-------------------------------------------//

//----------- Channel Details -------------//
unsigned long Channel_ID = 1437403; // Channel ID
const char * WriteAPIKey = "QGA1BXUCMLDNGYK7"; // Your write API Key
// ----------------------------------------//

WiFiClient  client;
unsigned long rr_millisBefore;


void setup() {
    Serial.begin(115200);
    while(!Serial)
  {
    ;
  }
  // put your setup code here, to run once:
  internet();
  ThingSpeak.begin(client);
  rr_millisBefore = millis();
  Serial.println("ready");
}



void loop() {

  
  StaticJsonBuffer <1000> jsonBuffer;
  JsonObject& root =jsonBuffer.parseObject(Serial);
  if(root == JsonObject::invalid())
    return;
  
  float ppm  = root["data1"];
  float ppm1 = root["data2"];
  Serial.println(ppm);
  Serial.println(ppm1);

if (millis() - rr_millisBefore > 15000)
  {
    Serial.println("uploading\n\n\n");
    upload(ppm,ppm1);
    rr_millisBefore = millis();
  }
}


void upload(float ppm,float ppm1)
{
  ThingSpeak.setField(1, ppm);
  ThingSpeak.setField(2, ppm1);
  

  internet();
 int x = ThingSpeak.writeFields(Channel_ID, WriteAPIKey);
  if (x == 200){Serial.println("Data Updated.");}
  else if (x != 200)
  {
    Serial.println("Data upload failed, retrying....");
    delay(7000);
    upload(ppm,ppm1);
  }
}


void internet()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nConnected.");
  }
}
