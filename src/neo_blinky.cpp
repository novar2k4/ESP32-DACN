#include "neo_blinky.h"

Adafruit_NeoPixel Connection(LED_COUNT_W, NEO_PIN_W, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel LED4(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

TaskHandle_t neoBlinkyTaskHandle = NULL;
float currentBrightness = 0.0f;
const float SMOOTH_STEP = 3.0f;
void neo_blinky(void *pvParameters)
{
    LED4.begin();

    LED4.clear();
    LED4.show();

    while (switch_state)
    {
        float light = global_light;
        float maxLight = max_light;
            
        if (light > max_light)
            light = max_light;

        float brightness = 255.0f * (1.0f - light / max_light);

        LED4.setBrightness(brightness);

        LED4.setPixelColor(
            0,
            LED4.Color(255, 20, 147)
        );

        LED4.show();

        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Khi switch_state = false
    // tắt LED trước khi task kết thúc
    LED4.clear();
    LED4.show();

    neoBlinkyTaskHandle = NULL;

    vTaskDelete(NULL);
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

void initNeoBlinky()
{
    if (neoBlinkyTaskHandle == NULL)
    {
        Serial.println("[NEO] Starting Neo Blinky task...");

        xTaskCreate(
            neo_blinky,
            "Task NEO Blink",
            2048,
            NULL,
            2,
            &neoBlinkyTaskHandle
        );
    }
}


void stopNeoBlinky()
{
    if (neoBlinkyTaskHandle != NULL)
    {
        Serial.println("[NEO] Stopping Neo Blinky task...");

        vTaskDelete(neoBlinkyTaskHandle);

        neoBlinkyTaskHandle = NULL;

        LED4.clear();
        LED4.show();
    }
}