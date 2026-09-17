#include "led_blinky.h"

// void led_blinky(void *pvParameters) {
//   pinMode(LED_GPIO, OUTPUT);
//   while(1) {                        
//     digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
//     vTaskDelay(100);
//     digitalWrite(LED_GPIO, LOW);   // turn the LED OFF
//     vTaskDelay(100);
//   }
// }

#define PIN_LED      48  // The onboard RGB NeoPixel pin
#define NUM_PIXELS    1  // There is only 1 build-in RGB LED

// Initialize the NeoPixel object
Adafruit_NeoPixel pixel(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);

void led_blinky(void *pvParameters) {
  pixel.begin();
  while(1) {
    // 1. Blink Red
    pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // R=255, G=0, B=0
    pixel.show();
    vTaskDelay(500);

    // 2. Blink Green
    pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // R=0, G=255, B=0
    pixel.show();
    vTaskDelay(500);

    // 3. Blink Blue
    pixel.setPixelColor(0, pixel.Color(0, 0, 255)); // R=0, G=0, B=255
    pixel.show();
    vTaskDelay(500);
    
    // Optional: Turn off briefly between cycles
    pixel.setPixelColor(0, pixel.Color(0, 0, 0)); 
    pixel.show();
    vTaskDelay(500);
  }
}

void initLED_Blinky() {
  xTaskCreate(led_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
}
