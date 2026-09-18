#include "led_blinky.h"

void led_blinky(void *pvParameters) {
  pinMode(LED_GPIO, OUTPUT);
  while(1) {                        
    digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
    vTaskDelay(100);
    digitalWrite(LED_GPIO, LOW);   // turn the LED OFF
    vTaskDelay(100);
  }
}


void initLED_Blinky() {
  xTaskCreate(led_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
}
