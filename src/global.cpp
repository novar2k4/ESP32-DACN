#include "global.h"

float global_temperature = 0;
float global_humidity = 0;
float global_light = 0;
int wifi = -1;
int lastBounds1 = 0;
int lastBounds2 = 0; 
int  glob_buttons1, glob_buttons2 = 0;
int s1 = 0;
int s2 = 0;
bool switch_state = 1;
float max_light = 330;
float global_ph = 0.0f;
void LEDWiFistatus();