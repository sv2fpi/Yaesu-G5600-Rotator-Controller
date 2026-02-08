/* * PROJECT: Yaesu Rotator Controller (ESP8266 Bridge)
 * AUTHOR: Sakis (SV2FPI)
 * HARDWARE: ESP-12E (ESP8266) & ST7735 TFT Display
 * COMPATIBILITY: Yaesu G-5400, G-5500, G-5600
 * * IMPORTANT NOTE: This code is part of a dual-device system and 
 * CANNOT work standalone. It must be used in conjunction with 
 * the corresponding Arduino Uno code.
 * * DESCRIPTION: This unit provides the WiFi/Blynk interface, the 
 * ST7735 TFT display UI, and handles communication with the 
 * Arduino Uno "Brain" via Serial commands.
 */

#define BLYNK_TEMPLATE_ID   "TMPL6w_Zi-VNj"
#define BLYNK_TEMPLATE_NAME "Yaesu Control"
#define BLYNK_AUTH_TOKEN    "i00r5icBokV2DqYYIkGPucEGMD90Zrry"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// TFT Display Pins
#define TFT_CS D8
#define TFT_RST D3 
#define TFT_DC D4
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Your_WiFi_SSID"; // User must enter their SSID
char pass[] = "Your_WiFi_Password"; // User must enter their Password

int currentAz = 0, currentEl = 0, lastAz = -1, lastEl = -1;
int targetAz = 0, targetEl = 0;
String serialData = "";
bool moveL = false, moveR = false, moveU = false, moveD = false;
BlynkTimer timer;

// Poll the Arduino Uno for data every 2 seconds
void askNano() { Serial.print("C\r"); }

// Manual Movement Control via Blynk
BLYNK_WRITE(V1) { moveL = param.asInt(); Serial.print(moveL ? "L\r" : "S\r"); drawArrows(); }
BLYNK_WRITE(V2) { moveR = param.asInt(); Serial.print(moveR ? "R\r" : "S\r"); drawArrows(); }
BLYNK_WRITE(V3) { moveU = param.asInt(); Serial.print(moveU ? "U\r" : "S\r"); drawArrows(); }
BLYNK_WRITE(V4) { moveD = param.asInt(); Serial.print(moveD ? "D\r" : "S\r"); drawArrows(); }

BLYNK_WRITE(V7) { targetAz = param.asInt(); }
BLYNK_WRITE(V8) { targetEl = param.asInt(); }

// GO Commands - Processing is handled by the Uno
BLYNK_WRITE(V9) { if(param.asInt()) { Serial.printf("W%d\r", targetAz); } }
BLYNK_WRITE(V10) { if(param.asInt()) { Serial.printf("W%d %d\r", currentAz, targetEl); } }
BLYNK_WRITE(V11) { if(param.asInt()) { Serial.print("S\r"); } }

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  
  // Welcome Screen
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2); 
  tft.setCursor(38, 40); tft.print("ANTENNA");
  tft.setCursor(38, 60); tft.print("ROTATOR");
  tft.setTextSize(1);
  tft.setCursor(53, 85); tft.print("by SV2FPI");
  delay(3000);

  drawUI();
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(2000, askNano);
}

void loop() {
  Blynk.run();
  timer.run();

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r' || c == '\n') {
      if (serialData.length() >= 10) {
        currentAz = serialData.substring(2, 5).toInt();
        currentEl = serialData.substring(7, 10).toInt();
        updateNumbers();
      }
      serialData = "";
    } else { serialData += c; }
  }

  // WiFi Status Dot
  static unsigned long lastWifi = 0;
  if (millis() - lastWifi > 5000) {
    uint16_t color = (WiFi.status() == WL_CONNECTED) ? ST77XX_GREEN : ST77XX_RED;
    tft.fillCircle(150, 12, 4, color);
    lastWifi = millis();
  }
}

void drawUI() {
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 25, ST77XX_BLUE);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(47, 8); tft.print("YAESU ROTOR");
  tft.drawRect(5, 30, 150, 45, ST77XX_WHITE);
  tft.setCursor(8, 33); tft.print("azimuth");
  tft.drawRect(5, 80, 150, 45, ST77XX_WHITE);
  tft.setCursor(8, 83); tft.print("elevation");
  drawArrows();
  lastAz = -1; lastEl = -1;
  updateNumbers();
}

void drawArrows() {
  tft.fillTriangle(15, 52, 25, 42, 25, 62, moveL ? ST77XX_RED : ST77XX_GREEN);
  tft.fillTriangle(145, 52, 135, 42, 135, 62, moveR ? ST77XX_RED : ST77XX_GREEN);
  tft.fillTriangle(20, 112, 10, 102, 30, 102, moveD ? ST77XX_RED : ST77XX_GREEN);
  tft.fillTriangle(140, 92, 130, 102, 150, 102, moveU ? ST77XX_RED : ST77XX_GREEN);
}

void updateNumbers() {
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_GREEN); 
  if (currentAz != lastAz) {
    tft.fillRect(45, 42, 75, 25, ST77XX_BLACK);
    tft.setCursor(55, 42); tft.printf("%03d", currentAz);
    lastAz = currentAz;
    Blynk.virtualWrite(V5, currentAz);
  }
  if (currentEl != lastEl) {
    tft.fillRect(45, 92, 75, 25, ST77XX_BLACK);
    tft.setCursor(55, 92); tft.printf("%03d", currentEl);
    lastEl = currentEl;
    Blynk.virtualWrite(V6, currentEl);
  }
}
