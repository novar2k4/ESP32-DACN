  #include <Arduino.h>
  #include <Adafruit_NeoPixel.h>
  #include "global.h" 

  #include "led_blinky.h"
  #include "neo_blinky.h"
  #include "dht.h"
  #include "LFS.h"
  #include "WebSocket.h"
  #include "CoreIoT.h"
  #include "LCD.h"

void setup() {
  Serial.begin(115200);
  initLED_Blinky();
  initNeoBlinky();
  // initDHT();
  initWiFi();
  // initLittleFS();
  // initWebSocket();
  // coreiot_init();
  initLCD();
  initLight();
  Button();
}

void loop() {
  // webSocketLoop();
  checkWiFistatus();
  // Update LCD display
  updateLCD();
  getButton();
  setButton();
}