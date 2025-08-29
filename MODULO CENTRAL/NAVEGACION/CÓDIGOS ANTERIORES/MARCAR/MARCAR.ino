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
int actualState = HOME;

bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

int selected = 0;

int seg = 0;
int ms = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);

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
}

void loop() {

  ReadButtonUp = digitalRead(BUTTON_UP);
  Serial.print("UP: ");
  if (ReadButtonUp == PUSHED) {
    Serial.print("PUSHED");
  } else {
    Serial.print("NOT PUSHED");
  }

  ReadButtonDown = digitalRead(BUTTON_DOWN);
  Serial.print(" | DOWN: ");
  if (ReadButtonDown == PUSHED) {
    Serial.print("PUSHED");
  } else {
    Serial.print("NOT PUSHED");
  }

  ReadButtonOk = digitalRead(BUTTON_OK);
  Serial.print(" | OK: ");
  if (ReadButtonOk == PUSHED) {
    Serial.print("PUSHED");
  } else {
    Serial.print("NOT PUSHED");
  }

  ReadButtonWater = digitalRead(BUTTON_WATER);
  Serial.print(" | WATER: ");
  if (ReadButtonWater == PUSHED) {
    Serial.println("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.setCursor(0, 0);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.setCursor(0, selected);
  lcd.print("*");
  lcd.setCursor(1, 0);
  lcd.print(" Planta 1");
  lcd.setCursor(1, 1);
  lcd.print(" Planta 2");

  switch (actualState) {
    case HOME:

      if (ReadButtonDown == PUSHED) {
        actualState = DOWN;
      }

      if (ReadButtonUp == PUSHED) {
        actualState = UP;
      }

      break;

    case DOWN:
      if (ReadButtonDown == N_PUSHED) {
        selected = selected + 1;
        actualState = HOME;
      }
      break;

    case UP:
      if (ReadButtonUp == N_PUSHED) {
        selected = selected - 1;
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
    ms = ms - 1000;
  }
  return;
}
