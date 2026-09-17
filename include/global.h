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

extern float global_temperature;
extern float global_humidity;
extern int wifi;
extern PubSubClient client;

#endif