#include "LFS.h"

void littleFS_task(void *pvParameters) {
    while(1) {
    if(!LittleFS.begin(true)){
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    File file = LittleFS.open("/text.txt");
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("File Content:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
    vTaskDelay(1000);
    }
}

void littleFS_init(){
    xTaskCreate(littleFS_task, "LittleFS Task", 4096, NULL, 1, NULL);
}




    