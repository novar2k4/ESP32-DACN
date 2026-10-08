#ifndef LIGHT_H
#define LIGHT_H
#include "global.h"
#include <NimBLEDevice.h>

bool isLightDevice(const NimBLEAdvertisedDevice *device);
bool connectLightSensor(const NimBLEAdvertisedDevice *device);

#endif // LIGHT_H
