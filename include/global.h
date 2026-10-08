#ifndef __GLOBAL_H__
#define __GLOBAL_H__

// LIBS
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <DHT20.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ArduinoJSON.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <PubSubClient.h>
#include "LiquidCrystal_I2C.h"
#include <Wire.h> 
#include <NimBLEDevice.h>

// Include H 
#include "led_blinky.h"
#include "neo_blinky.h"
#include "LFS.h"
#include "WebSocket.h"
#include "CoreIoT.h"
#include "BLE.h"
// #include "BLE.cpp"

#include "Light.h"
#include "TempHumid.h"
#include "Water_Quality.h"
#include "Sensors.h"

extern float global_temperature;
extern float global_humidity;
extern float global_light;
extern int wifi;
extern int s1,s2;
extern int lastBounds1,lastBounds2;
extern int  glob_buttons2, glob_buttons1;
extern PubSubClient client;
extern NimBLEClient* client1;
extern bool switch_state;
extern float max_light;
extern float global_ph;
extern int global_ec;


void LEDWiFistatus();

#endif  