  #include "global.h" 

void setup() {
  Serial.begin(115200);
  // initBLEStatusTask();
  // initLED_Blinky();
  initWiFi();
  // initLittleFS();
  // initWebSocket();
  coreiot_init();
  BLE1();
  initHumid();
  initBLEStatusTask();
}

void loop() {
  // webSocketLoop();
  relayTask();
  LEDWiFistatus();
  checkWiFistatus();  
 
  //updateLEDFromLight();
}

