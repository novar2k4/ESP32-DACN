#include "dht.h"
DHT20 DHT;
int count = 0;
void dht_task(void *pvParameters) {
    Wire.begin(11, 12);
    DHT.begin();

    while(1) {
        DHT.read();
        global_temperature = DHT.getTemperature();
        global_humidity = DHT.getHumidity();
        Serial.print("Temperature: ");
        Serial.print(global_temperature, 1);
        Serial.print("°C     ");
        Serial.print("Humidity: ");
        Serial.print(global_humidity, 1);
        Serial.println("%");  
        vTaskDelay(3000);
        
    } 
}

void initDHT() {
    xTaskCreate(dht_task, "DHT Task", 2048, NULL, 2, NULL);
}