#include "Sensors.h"

#define SCREEN_5IN1 1

void setSwitchState(bool state)
{
    switch_state = state;
}

void setMaxLight(float value)
{
    // Avoid divide-by-zero in the NeoPixel brightness calculation.
    if (value <= 0.0f)
        value = 500.0f;

    max_light = value;
}

//// RELAY ////
void screenTask(){
    pinMode(SCREEN_5IN1, OUTPUT);
    // digitalWrite(SCREEN_5IN1, LOW);
    Serial.print("Water Quality Sensor Missing: ");
    Serial.println(water_state ? "YES" : "NO");
        if (water_state){
            digitalWrite(SCREEN_5IN1, HIGH);
            delay(50);
            digitalWrite(SCREEN_5IN1, LOW);
            delay(3000);
        }
        else digitalWrite(SCREEN_5IN1, LOW);
}


