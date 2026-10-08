#ifndef LIGHT_H
#define LIGHT_H
#include "global.h"


bool isLightDevice(const NimBLEAdvertisedDevice *device);
bool connectLightSensor(const NimBLEAdvertisedDevice *device);
bool isLightConnected();
static const char *LIGHT_DEVICE_NAME = "inno-013-db8d";
static const char *LIGHT_SENSOR_ADDRESS = "0c:8b:95:fa:42:fe";

#endif // LIGHT_H
