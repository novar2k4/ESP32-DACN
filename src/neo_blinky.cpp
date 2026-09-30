#include "neo_blinky.h"

Adafruit_NeoPixel Connection(LED_COUNT_W, NEO_PIN_W, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LED4(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

void neo_blinky(void *pvParameters)
{
    LED4.begin();
    // Connection.begin();

    LED4.clear();
    // Connection.clear();
    LED4.show();
    // Connection.show();

    while (1)
    {
        float light = global_light;
        if (light > max_light ) light = max_light;

        float brightness = 255.0f * (1.0f - light / max_light);

        LED4.setBrightness(brightness);

        LED4.setPixelColor(0, LED4.Color(255, 20, 147));
        
        LED4.show();
        Serial.print("LIGHT: ");
        Serial.println(light);
        Serial.print("Brightness: ");
        Serial.println(brightness);
        vTaskDelay(50);
    }
}

void LEDWiFistatus()
{
    Connection.begin();
    Connection.clear();
    Connection.show();

    if (wifi == -1)
    {
        Connection.setPixelColor(0, Connection.Color(255, 0, 0)); // Set pixel 0 to red
        Connection.setBrightness(10);                             // Set brightness to 10 max 255
        Connection.show();                                        // Update the strip
        vTaskDelay(500);
        Connection.clear();
        Connection.show();
        vTaskDelay(500);
    }
    else if (wifi == 0)
    {
        Connection.setPixelColor(0, Connection.Color(255, 255, 0)); // Set pixel 0 to yellow
        Connection.setBrightness(10);                               // Set brightness to 10
        Connection.show();                                          // Update the strip
        vTaskDelay(500);
        Connection.clear();
        Connection.show();
        vTaskDelay(500);
    }
    else
    {
        Connection.setPixelColor(0, Connection.Color(0, 255, 0)); // Set pixel 0 to green
        Connection.setBrightness(10);                             // Set brightness to 10
        Connection.show();                                        // Update the strip
        vTaskDelay(500);
        Connection.clear();
        Connection.show();
        vTaskDelay(500);
    }
}

void updateLEDFromLight()
{
    float light = global_light;
    // -----------------------------------------
    // Inverse mapping
    // 0 lux   -> 255 brightness
    // 4095 lux -> 0 brightness
    // -----------------------------------------

    uint8_t brightness =
        (uint8_t)(255.0f - (light / 4095.0f) * 255.0f);

    // -----------------------------------------
    // Set brightness
    // -----------------------------------------

    LED4.setBrightness(brightness);

    // -----------------------------------------
    // Set LED colors
    // -----------------------------------------
    LED4.setPixelColor(
        0,
        LED4.Color(0, 0, 255));
    LED4.show();
    Serial.print(" lux -> Brightness: ");
    Serial.println(brightness);
}

void initNeoBlinky()
{
    xTaskCreate(neo_blinky, "Task NEO Blink", 2048, NULL, 2, NULL);
}