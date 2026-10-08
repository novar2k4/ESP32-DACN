#ifndef SENSORS_H
#define SENSORS_H
#include "global.h"

void setSwitchState(bool state);
void setMaxLight(float value);
// void initRelay();
void relayTask();
#endif // SENSORS_H
