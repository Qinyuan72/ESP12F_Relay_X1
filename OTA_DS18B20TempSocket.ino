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
int TimeOnLoopCount   = 0;
int TimeOnLoopCount_min = 0;
int Delaytime = 20;//milliseconds

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
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  // Initialize the output variables as outputs
  //pinMode(output5, OUTPUT);
  //pinMode(output4, OUTPUT);
  // Set outputs to LOW
  // digitalWrite(output5, LOW);
  //digitalWrite(output4, LOW);

  //    Connect to Wi-Fi network with SSID and password
  Wire.begin(0, 2);
  lcd.begin();
  lcd.print("LCD,Initalized.");
  delay(1000);
  lcd.clear();
  lcd.print("Wi-Fi connected                         ");
  lcd.print("IP");
  lcd.print(WiFi.localIP());
  wifiServer.begin();

  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);
  sensors.begin();

  pinMode(output5, OUTPUT);
  while (TimeHaltLoopCount > 0)
  {
    digitalWrite(output5, LOW);
    ArduinoOTA.handle();
    TimeHaltLoopCount = TimeHaltLoopCount - 1;
    lcd.clear();
    lcd.print("StartIn: ");
    lcd.print(TimeHaltLoopCount);
    lcd.print(" Sec");
    lcd.setCursor(0 , 1);
    lcd.print(WiFi.localIP());
    delay(1000);
  }

}


void loop() {
  ArduinoOTA.handle();


  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  /********************************************************************/
  TimeOnLoopCount = TimeOnLoopCount + 1;
  Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperature readings
  Serial.println("DONE");
  lcd.clear();
  lcd.print("Temp:");
  lcd.print(sensors.getTempCByIndex(0));
  /********************************************************************/
  Serial.print("Temperature is: ");
  lcd.print(" OFF");
  Serial.print(sensors.getTempCByIndex(0)); // Why "byIndex"?
  // You can have more than one DS18B20 on the same bus.
  // 0 refers to the first IC on the wire
  if (sensors.getTempCByIndex(0) > -15) {
    digitalWrite(output5, HIGH);
    delay(Delaytime);
    while (sensors.getTempCByIndex(0) > -20)
    {
      TimeOnLoopCount = TimeOnLoopCount + 1;
      ArduinoOTA.handle();
      digitalWrite(output5, HIGH);
      sensors.requestTemperatures();
      lcd.clear();
      lcd.print("Temp:");
      lcd.print(sensors.getTempCByIndex(0));
      lcd.print(" ON");
      lcd.setCursor(0 , 1);
      if (TimeOnLoopCount % 2 == 1) {
        lcd.print(WiFi.localIP());
      }
      else {
        lcd.print("Time:");
        TimeOnLoopCount_min = TimeOnLoopCount / 60;
        lcd.print(TimeOnLoopCount_min);
        lcd.print('M');
        lcd.print(TimeOnLoopCount % 60);
        lcd.print('S');
        delay(100);
      }
      delay(1000);
    }
    TimeOnLoopCount = 0;
  }
  digitalWrite(output5, LOW);
  lcd.setCursor(0 , 1);
  if (TimeOnLoopCount % 2 == 1) {
    lcd.print(WiFi.localIP());
  }
  else {
    lcd.print("Time:");
    TimeOnLoopCount_min = TimeOnLoopCount / 60;
    lcd.print(TimeOnLoopCount_min);
    lcd.print('M');
    lcd.print(TimeOnLoopCount % 60);
    lcd.print('S');
  }
  delay(1000);
}