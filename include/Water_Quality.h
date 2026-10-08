#ifndef WATER_QUALITY_H
#define WATER_QUALITY_H
#include "global.h"
#include <NimBLEDevice.h>

bool isWaterQualityDevice(const NimBLEAdvertisedDevice *device);
bool connectWaterQualitySensor(const NimBLEAdvertisedDevice *device);

#endif // WATER_QUALITY_H
