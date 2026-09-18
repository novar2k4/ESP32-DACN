#ifndef __GLOBAL_H__
#define __GLOBAL_H__


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
#include "Light.h"
#include "DualButton.h"

extern float global_temperature;
extern float global_humidity;
extern float global_light;
extern int wifi;
extern int s1,s2;
extern int lastBounds1,lastBounds2;
extern PubSubClient client;

#endif