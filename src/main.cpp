  #include <Arduino.h>
  #include <Adafruit_NeoPixel.h>
  #include "global.h" 

  #include "led_blinky.h"
  #include "neo_blinky.h"
  #include "dht.h"
  #include "LFS.h"
  #include "WebSocket.h"
  #include "CoreIoT.h"
void setup() {
  Serial.begin(115200);
  // initLED_Blinky();
  initNeoBlinky();
  initDHT();
  // delay(2000); // Wait for 2 seconds before starting CoreIoT
  // Serial.println("Starting Wifi...");
  initWiFi();
  // initLittleFS();
  // initWebSocket();
  // delay(2000); // Wait for 2 seconds before starting CoreIoT
  // Serial.println("Starting CoreIoT...");
  coreiot_init();
}

void loop() {
  // webSocketLoop();
  checkWiFistatus();
}