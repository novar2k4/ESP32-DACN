  #include "global.h" 

void setup() {
  Serial.begin(115200);
  // initLED_Blinky();
  initWiFi();
  // initLittleFS();
  // initWebSocket();
  coreiot_init();
  BLE1();
}

void loop() {
  // webSocketLoop();
  LEDWiFistatus();
  checkWiFistatus();  
  //updateLEDFromLight();
}

