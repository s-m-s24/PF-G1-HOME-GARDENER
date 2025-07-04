#include <LiquidCrystal_I2C.h>

//21: SDA
//22: SLC

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

struct Plant {
  String name;
  int temp;
  int humid;
};
Plant tomate = {"tomate", 1, 2};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
    lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("");
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print("Name: ");
  Serial.println(tomate.name);
  Serial.print("Temp: ");
  Serial.println(tomate.temp);
  Serial.print("Humid: ");
  Serial.println(tomate.humid);
  lcd.setCursor(0,0);
  lcd.print(tomate.name);
  lcd.setCursor(0,1);
  lcd.print(tomate.temp);
  lcd.setCursor(3,1);
  lcd.print(tomate.humid);
}
