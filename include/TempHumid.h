#ifndef TEMP_HUMID_H
#define TEMP_HUMID_H
#include "global.h"
#include <NimBLEDevice.h>

bool isTempDevice(const NimBLEAdvertisedDevice *device);
bool connectTempSensor(const NimBLEAdvertisedDevice *device);
void initHumid();
bool isTemperatureConnected();
static const char *TEMP_DEVICE_NAME = "inno-004-8645";
static const char *TEMP_SENSOR_ADDRESS = "08:a6:f7:07:79:e6";
#endif // TEMP_HUMID_H
