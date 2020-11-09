#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

bool inrange = false;

const char* ssid = "CMCC-201";
const char* password = "18639289390";


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



WiFiServer wifiServer(8080);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

WiFiServer server(80);


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  Wire.begin(0, 2);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("OLED,Initalized.");
  display.display();
  delay(1000);

  display.fillRect(0, 0, 128, 32, WHITE);
  display.display();
  delay(1000);
  display.display();
  display.setCursor(0, 0);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.println("Conncting WiFi");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("No wifi~~~~");
    delay(5000);
    break;
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



  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Wi-Fi connected");
  display.println(WiFi.localIP());
  display.display();
  wifiServer.begin();
  delay(5000);

  OneWire oneWire(ONE_WIRE_BUS);
  DallasTemperature sensors(&oneWire);
  sensors.begin();

  pinMode(output5, OUTPUT);
  digitalWrite(output5, LOW);

  while (TimeHaltLoopCount > 0)
  {
    TimeHaltLoopCount = TimeHaltLoopCount - 1;
    ArduinoOTA.handle();
    display.clearDisplay();
    display.setCursor(58, 0);
    display.setTextSize(3);
    display.println(TimeHaltLoopCount);
    display.setTextSize(1);
    display.setCursor(25, 25);
    display.println(WiFi.localIP());
    display.display();
    Serial.println(TimeHaltLoopCount);
    delay(1000);
  }

}


void loop() {
  ArduinoOTA.handle();
  inrange = false;

  TimeOnLoopCount = TimeOnLoopCount + 1;
  sensors.requestTemperatures(); // Send the command to get temperature readings
  oldedisplay(inrange);

  if (sensors.getTempCByIndex(0) > RangeTop) {
    digitalWrite(output5, HIGH);
    inrange = true;
    delay(Delaytime);
    TimeOnLoopCount = 0;//Reset TimeOnLoopCount
    LoopCount = LoopCount + 1;//loopCount + 1, This is the window to count the loop
    while (sensors.getTempCByIndex(0) > Rangebottom)
    {
      TimeOnLoopCount = TimeOnLoopCount + 1;
      ArduinoOTA.handle();
      digitalWrite(output5, HIGH);
      sensors.requestTemperatures();
      oldedisplay(inrange);
      delay(1000);
    }
    TimeOnLoopCount = 0;
  }
  delay(1000);
  digitalWrite(output5, LOW);
}

int oldedisplay(bool i) {
  display.clearDisplay();
  display.setCursor(0, 5);
  display.setTextSize(3);
  display.println(sensors.getTempCByIndex(0));
  display.setTextSize(3);
  display.drawCircle(128 - 16, 16, 10, WHITE);

  Serial.print("Temp: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print(" C     LocalIP: ");
  Serial.print(WiFi.localIP());

  if (i == true) {
    display.fillCircle(128 - 16, 16, 10, WHITE);
    Serial.println("    Status: ON ");
  }
  else
  {
    Serial.println("    Status: Off");
  }
  display.display();
}