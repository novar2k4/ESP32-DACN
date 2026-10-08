  #include "global.h" 

void setup() {
  Serial.begin(115200);
  // initBLEStatusTask();
  // initLED_Blinky();
  relayTask();
  initWiFi();
  // initLittleFS();
  // initWebSocket();
  coreiot_init();
  BLE1();
  initHumid();
  // initRelay();
}

void loop() {
  // webSocketLoop();
  LEDWiFistatus();
  checkWiFistatus();  
 
  //updateLEDFromLight();
}

