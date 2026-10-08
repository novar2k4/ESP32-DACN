#ifndef WATER_QUALITY_H
#define WATER_QUALITY_H
#include "global.h"

bool isWaterQualityDevice(const NimBLEAdvertisedDevice *device);
bool connectWaterQualitySensor(const NimBLEAdvertisedDevice *device);
bool isWaterQualityConnected();
static const char *PH_DEVICE_NAME = "BLE-9909";
static const char *PH_SENSOR_ADDRESS = "bc:96:51:5b:d9:ec";
#endif // WATER_QUALITY_H
