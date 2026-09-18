#include "DualButton.h"

#define BUTTON_S1 6
#define BUTTON_S2 7

void Button(){
    pinMode(BUTTON_S1, INPUT_PULLUP);
    pinMode(BUTTON_S2, INPUT_PULLUP);
}

void setButton(){ 
       
    if(s1 == 0){
        if (lastBounds1 == 0) {
            lastBounds1 = 1;
        }
        else lastBounds1 = 0;
    }

    if(s2 == 0){
        if (lastBounds2 == 0){
            lastBounds2 = 1;
        }
        else lastBounds2 = 0;
    }
    Serial.print("S1: ");
    Serial.print(lastBounds1);
    Serial.print(" |S2: ");
    Serial.println(lastBounds2);

    delay(100);
}

void getButton(){
    s1 = digitalRead(BUTTON_S1);
    s2 = digitalRead(BUTTON_S2);
    delay(100);
}

