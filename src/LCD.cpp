#include "LCD.h"

LiquidCrystal_I2C lcd(0x21, 16, 2);

void initLCD()
{
    // Dùng chung I2C với DHT20
    // Wire.begin(11, 12);

    lcd.init();
    lcd.backlight();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("H:");
    lcd.print(global_humidity);
    // lcd.print(" %");

    lcd.setCursor(8, 0);
    lcd.print("T:");
    lcd.print(global_temperature);
    // lcd.print(" C");
    Serial.println("LCD initialized");

    lcd.setCursor(0, 1);
    lcd.print("L:");
    lcd.print(global_light);
    // lcd.print(" %");

    lcd.setCursor(8, 1);
    lcd.print("stat: ");

}

void updateLCD()
{
    lcd.setCursor(2, 0);
    lcd.print(global_humidity);
    // lcd.print(" %");

    lcd.setCursor(10, 0);
    lcd.print(global_temperature);
    // lcd.print(" C");

    lcd.setCursor(2, 1);
    lcd.print(global_light);
    // lcd.print(" %");
}