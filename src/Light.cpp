#include "Light.h"

#include <Arduino.h>
#include "global.h"

#define LIGHT_PIN 1
#define max_light 4095
void light_task(void *pvParameters)
{
    pinMode(LIGHT_PIN, INPUT);

    while (1)
    {
        float value = analogRead(LIGHT_PIN);

        global_light = (value*100)/max_light;

        Serial.print("Light: ");
        Serial.print(global_light);
        Serial.println("%");

        vTaskDelay(1000);
    }
}

void initLight()
{
    xTaskCreate(light_task,"Light Task",2048,NULL,2,NULL);
}