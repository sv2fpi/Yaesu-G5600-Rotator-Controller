/* * PROJECT: Yaesu Rotator Controller (Uno Smart Brain)
 * AUTHOR: Sakis (SV2FPI)
 * HARDWARE: Arduino Uno & Relay Shield
 * COMPATIBILITY: Yaesu G-5400, G-5500, G-5600
 * * IMPORTANT NOTE: This code is part of a dual-device system and 
 * CANNOT work standalone. It must be used in conjunction with 
 * the corresponding ESP8266 code.
 * * DESCRIPTION: This unit handles the relay control and the 180-540 
 * degree internal mapping (South-to-South) to ensure smooth 
 * North-crossing (0 degrees) without mechanical or logical locks.
 */

#include <SoftwareSerial.h>

// Communication with ESP8266 (Pins 4 RX, 5 TX)
SoftwareSerial espSerial(4, 5); 

const int relayUp = 8, relayDown = 9, relayLeft = 10, relayRight = 11;
int targetAz = -1, targetEl = -1;
const int tolerance = 2; 

bool mLeft = false, mRight = false, mUp = false, mDown = false;

void setup() {
  delay(1000); 
  Serial.begin(9600);    
  espSerial.begin(9600); 
  pinMode(relayUp, OUTPUT); pinMode(relayDown, OUTPUT);
  pinMode(relayLeft, OUTPUT); pinMode(relayRight, OUTPUT);
  digitalWrite(relayUp, HIGH); digitalWrite(relayDown, HIGH);
  digitalWrite(relayLeft, HIGH); digitalWrite(relayRight, HIGH);
}

void processCommand(char c, Stream &source) {
  if (c == 'C') {
    if (source.peek() == '2') source.read(); 
    int curAzRaw = (int)constrain(map(analogRead(A0), 49, 1023, 180, 540), 180, 540);
    int curElRaw = constrain(map(analogRead(A1), 0, 1023, 0, 180), 0, 180);
    char buffer[20];
    sprintf(buffer, "+0%03d+0%03d\r\r", curAzRaw % 360, curElRaw); 
    source.print(buffer);
  }
  else if (c == 'L' || c == 'R' || c == 'U' || c == 'D' || c == 'S') {
    if (c == 'L') { mLeft = true; mRight = false; targetAz = -1; }
    else if (c == 'R') { mRight = true; mLeft = false; targetAz = -1; }
    else if (c == 'U') { mUp = true; mDown = false; targetEl = -1; }
    else if (c == 'D') { mDown = true; mUp = false; targetEl = -1; }
    else if (c == 'S') { targetAz = -1; targetEl = -1; mLeft = mRight = mUp = mDown = false; }
    source.print("\r");
  }
  else if (c == 'W') {
    delay(50);
    String cmd = source.readStringUntil('\r');
    cmd.trim();
    
    int spaceIndex = cmd.indexOf(' ');
    if (spaceIndex != -1) {
      int valAz = cmd.substring(0, spaceIndex).toInt();
      int valEl = cmd.substring(spaceIndex + 1).toInt();
      targetAz = (valAz < 180) ? valAz + 360 : valAz;
      targetEl = valEl;
    } else {
      int valAz = cmd.toInt();
      targetAz = (valAz < 180) ? valAz + 360 : valAz;
    }
    mLeft = mRight = mUp = mDown = false;
    source.print("\r");
  }
}

void loop() {
  if (Serial.available() > 0) processCommand(Serial.read(), Serial);
  if (espSerial.available() > 0) processCommand(espSerial.read(), espSerial);

  int curAz = (int)constrain(map(analogRead(A0), 49, 1023, 180, 540), 180, 540);
  int curEl = constrain(map(analogRead(A1), 0, 1023, 0, 180), 0, 180);

  bool moveL = mLeft, moveR = mRight, moveU = mUp, moveD = mDown;
  if (targetAz != -1) {
    if (curAz < (targetAz - tolerance)) { moveR = true; moveL = false; }
    else if (curAz > (targetAz + tolerance)) { moveL = true; moveR = false; }
    else { targetAz = -1; moveR = moveL = false; }
  }
  if (targetEl != -1) {
    if (curEl < (targetEl - tolerance)) { moveU = true; moveD = false; }
    else if (curEl > (targetEl + tolerance)) { moveD = true; moveU = false; }
    else { targetEl = -1; moveU = moveD = false; }
  }
  digitalWrite(relayLeft, moveL ? LOW : HIGH);
  digitalWrite(relayRight, moveR ? LOW : HIGH);
  digitalWrite(relayUp, moveU ? LOW : HIGH);
  digitalWrite(relayDown, moveD ? LOW : HIGH);
}
