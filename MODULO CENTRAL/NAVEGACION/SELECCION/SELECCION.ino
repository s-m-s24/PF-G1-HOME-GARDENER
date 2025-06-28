#include <List.hpp>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>  //21: SDA || 22: SLC

#define BUTTON_UP 25
#define BUTTON_DOWN 26
#define BUTTON_OK 27
#define BUTTON_WATER 33

#define LED_WATER 4
#define LED_LOADING 15
#define LED_ON 5

#define PUSHED LOW
#define N_PUSHED HIGH

#define HOME 1
#define DOWN 2
#define UP 3
#define PRINT 4
int actualState = HOME;

bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

List<String> plants;

int selected = 0;
int printIndex = 0;

int seg = 0;
int ms = 0;
int segPrint = 0;
#define T_PRINT 1

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);

  plants.add("Tomate");
  plants.add("Pepino");
  plants.add("Remolacha");
  plants.add("Rúcula");

  Timer1.initialize(1000);            //Cada cuantos milisegundos queremos que interrumpa el timer, en este caso: 1000
  Timer1.attachInterrupt(ISR_Timer);  //A dónde queremos ir en la interrupción, en este caso: ISR_Timer (la función más abajo);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);
  pinMode(BUTTON_WATER, INPUT_PULLUP);

  pinMode(LED_WATER, OUTPUT);
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_LOADING, OUTPUT);

  // Inicializar el LCD
  lcd.init();

  //Encender la luz de fondo.
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("*");
  lcd.setCursor(1, 0);
  lcd.print(plants.get(printIndex));
  lcd.setCursor(1, 1);
  lcd.print(plants.get(printIndex + 1));
}

void loop() {

  ReadButtonUp = digitalRead(BUTTON_UP);
  ReadButtonDown = digitalRead(BUTTON_DOWN);
  ReadButtonOk = digitalRead(BUTTON_OK);
  ReadButtonWater = digitalRead(BUTTON_WATER);

  /*
  Serial.print("UP: ");
  if (ReadButtonUp == PUSHED) {
    Serial.print("PUSHED");
  } else {
    Serial.print("NOT PUSHED");
  }
  
  Serial.print(" | DOWN: ");
  if (ReadButtonDown == PUSHED) {
    Serial.print("PUSHED");
  } else {
    Serial.print("NOT PUSHED");
  }

  Serial.print(" | OK: ");
  if (ReadButtonOk == PUSHED) {
    Serial.print("PUSHED");
  } else {
    Serial.print("NOT PUSHED");
  }

  Serial.print(" | WATER: ");
  if (ReadButtonWater == PUSHED) {
    Serial.println("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }
  */

  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)

  switch (actualState) {
    case HOME:
      lcd.setCursor(1, 0);
      lcd.print(plants.get(printIndex));
      lcd.setCursor(1, 1);
      lcd.print(plants.get(printIndex + 1));
      if (ReadButtonDown == PUSHED) {
        actualState = DOWN;
        Serial.println(selected);
      }

      if (ReadButtonUp == PUSHED) {
        actualState = UP;
        Serial.println(selected);
      }

      break;

    case DOWN:
      if (ReadButtonDown == N_PUSHED) {
        lcd.setCursor(1, 0);
        lcd.print("");
        lcd.setCursor(1, 1);
        lcd.print("");
        if (selected = 1) {
          printIndex = printIndex + 1;
          selected = 0;
          lcd.setCursor(0, 0);
          lcd.print("*");
          lcd.setCursor(0, 1);
          lcd.print("");
          actualState = HOME;
          Serial.print(selected);
          Serial.print(" | ");
          Serial.println(printIndex);
        }
        else if (selected = 0)
        {
          selected = 1;
          lcd.setCursor(0, 0);
          lcd.print("");
          lcd.setCursor(0, 1);
          lcd.print("*");
          Serial.print(selected);
          Serial.print(" | ");
          Serial.println(printIndex);
          actualState = HOME;
        }
      }
      break;

    case UP:
      if (ReadButtonUp == N_PUSHED) {
        lcd.setCursor(0, 0);
        lcd.print("");
        lcd.setCursor(0, 1);
        lcd.print("");
        lcd.setCursor(1, 0);
        lcd.print("");
        lcd.setCursor(1, 1);
        lcd.print("");
        if (selected = 0) {
          printIndex = printIndex - 1;
          selected = 1;
          actualState = HOME;
        }
        else
        {
          selected = 0;
          actualState = HOME;
        }
        Serial.print(selected);
        Serial.print(" | ");
        Serial.println(printIndex);
      }
      break;

    case PRINT:
      if (ReadButtonUp == N_PUSHED) {
        actualState = HOME;
      }
      break;
  }
}

void ISR_Timer(void)  //Crea la función de qué va a hacer cuando se produzca una interrupción
{
  ms = ms + 1;
  if (ms >= 1000) {
    seg = seg + 1;
    segPrint = segPrint + 1;
    ms = ms - 1000;
  }
  return;
}
