#ifndef LCD_H
#define LCD_H

#include "global.h"

void initLCD();
void displayLCD(const char* line1, const char* line2);
void updateLCD();

#endif // LCD_H