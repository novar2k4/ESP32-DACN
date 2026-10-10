#include "Sensors.h"

#define RELAY_PIN 6

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
void relayTask(){
    pinMode(RELAY_PIN, OUTPUT);
    // digitalWrite(RELAY_PIN, LOW);
    Serial.print("[RELAY] Water Quality Sensor Missing: ");
    Serial.println(water_state ? "YES" : "NO");
        if (water_state){
            digitalWrite(RELAY_PIN, HIGH);
            delay(50);
            digitalWrite(RELAY_PIN, LOW);
            delay(5000);
        }
        else digitalWrite(RELAY_PIN, LOW);
}


