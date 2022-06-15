#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Line 1 test OK");
  lcd.setCursor(0, 1);
  lcd.print("Line 2 test OK");
}

void loop()
{
}
