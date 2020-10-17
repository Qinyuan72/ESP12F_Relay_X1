#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#ifndef STASSID
#define STASSID "Xiaomi_3D3E"
#define STAPSK  "2019newpassword"
#endif
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


const char* ssid = "Xiaomi_3D3E";
const char* password = "2019newpassword";

//const int output5 = 16;  //Comment for testing,
const int output5 = 5; //Un-comment this when rumming.


int TimeHaltLoopCount = 180;// in second, should be set to 180s.
int TimeOnLoopCount     = 0;
int TimeOnLoopCount_min = 0;
int TimeOnLoopCount_h   = 0;
int LoopCount = 0;
int Delaytime = 20;//milliseconds
const int RangeTop    = -15;
const int Rangebottom = -20;
int status


WiFiServer wifiServer(8080);
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

WiFiServer server(80);


void setup() {
  Serial.begin(115200);
  ConnctToWiFi();
}


bool ConnctToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password)
    while (i < 16){
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            lcd.print(".");
            Serial.print(".");
        } 
        i = i + 1
     }
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        return false;
        }
    return true; 
}

