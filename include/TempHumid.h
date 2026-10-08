#ifndef TEMP_HUMID_H
#define TEMP_HUMID_H
#include "global.h"
#include <NimBLEDevice.h>

bool isTempDevice(const NimBLEAdvertisedDevice *device);
bool connectTempSensor(const NimBLEAdvertisedDevice *device);
void initHumid();
#endif // TEMP_HUMID_H
