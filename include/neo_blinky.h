#ifndef _NEO_BLINKY_H_
#define _NEO_BLINKY_H_

#include "global.h"

#define NEO_PIN 8 // Device Neopixel
#define LED_COUNT 4 // Device LED Count

#define NEO_PIN_W 45 // YOLO UNO Neopixel
#define LED_COUNT_W 1 // YOLO UNO LED 

extern TaskHandle_t neoBlinkyTaskHandle;

void initNeoBlinky();
void stopNeoBlinky();
void neo_blinky();
void LEDWiFistatus();
// void updateLEDFromLight();
#endif // NEO_BLINKY_H