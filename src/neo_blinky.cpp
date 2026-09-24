#include "neo_blinky.h"

    Adafruit_NeoPixel Connection(LED_COUNT_W, NEO_PIN_W, NEO_GRB + NEO_KHZ800);
    Adafruit_NeoPixel LED4(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

void neo_blinky(void *pvParameters) {
    LED4.begin();
    Connection.begin();
    LED4.clear();
    Connection.clear();
    LED4.show();
    Connection.show();
    while(1){
    Serial.println("GREEN");
        LED4.setPixelColor(0, LED4.Color(0, 255, 0));
        LED4.show();
        delay(1000);
        Serial.println("RED");
        LED4.setPixelColor(0, LED4.Color(255, 0, 0));
        LED4.show();
        delay(1000);

    }

}

void LEDWiFistatus(){
    Connection.begin();
    Connection.clear();
    Connection.show(); 


    if (wifi==-1){              
        Connection.setPixelColor(0, Connection.Color(255, 0, 0)); // Set pixel 0 to red
        Connection.setBrightness(10); // Set brightness to 10 max 255 
        Connection.show(); // Update the strip
        vTaskDelay(500);
        Connection.clear();
        Connection.show();
        vTaskDelay(500);
    } else if (wifi==0){
        Connection.setPixelColor(0, Connection.Color(255, 255, 0)); // Set pixel 0 to yellow
        Connection.setBrightness(10); // Set brightness to 10
        Connection.show(); // Update the strip
        vTaskDelay(500);
        Connection.clear();
        Connection.show();
        vTaskDelay(500);
    } else {
        Connection.setPixelColor(0, Connection.Color(0, 255, 0)); // Set pixel 0 to green
        Connection.setBrightness(10); // Set brightness to 10
        Connection.show(); // Update the strip
        vTaskDelay(500);
        Connection.clear();
        Connection.show();
        vTaskDelay(500);
    }
}

void initNeoBlinky() {
  xTaskCreate(neo_blinky, "Task NEO Blink", 2048, NULL, 2, NULL);
}