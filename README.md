# Yaesu Rotator Controller (G-5400/5500/5600)
**Developed by Sakis (SV2FPI)**

This is a smart, dual-microcontroller rotator controller designed to replace or enhance the manual control of Yaesu satellite rotators. It solves the "North Rollover" issue by using a logical 180°-540° mapping.

## Hardware Required
* **Arduino Uno**: The "Brain" (Controls Relays and Voltage Mapping).
* **ESP8266 (ESP-12E)**: The "Bridge" (Handles Blynk IoT, WiFi, and TFT Display).
* **Relay Module (4-Channel)**: For Up, Down, Left, Right movement.
* **ST7735 TFT Display**: For real-time Azimuth and Elevation feedback.

## Features
* **Seamless 360° Transition**: Moves through 0° (North) without stopping, using an internal 180°-540° scale.
* **Dual Control**: Supports both Blynk IoT app and PC software (like HRD) via Serial commands.
* **Real-time Feedback**: Visual display of current position on the TFT screen and the mobile app.

## Connection Setup
1. **Uno Pins 4 (RX) & 5 (TX)** connect to **ESP8266 TX/RX** (use voltage divider if necessary).
2. **Uno Analog A0**: Azimuth Voltage from Yaesu Controller.
3. **Uno Analog A1**: Elevation Voltage from Yaesu Controller.
4. **Relay Pins (Uno)**: 8 (Up), 9 (Down), 10 (Left), 11 (Right).

## Setup Instructions
1. Flash `Uno_Smart_Brain.ino` to your Arduino Uno.
2. Flash `ESP8266_Bridge.ino` to your ESP8266 after entering your WiFi and Blynk credentials.
3. Open the Blynk app and map the Virtual Pins as defined in the code (V1-V11).

---
*73 de SV2FPI*
