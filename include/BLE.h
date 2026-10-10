#ifndef BLE_H
#define BLE_H
#include "global.h"


void BLE1();
void printBLEStatus();
void reconnectBLEDevices();
void BLEStatusTask(void *pvParameters);
void initBLEStatusTask();
#endif // BLE_H
