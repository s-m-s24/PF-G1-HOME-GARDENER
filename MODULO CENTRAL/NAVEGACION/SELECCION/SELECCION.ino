#include <List.hpp>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>  //21: SDA || 22: SLC

struct Plant {
  String plantName;
  int temp;
  int humid;

};

List<Plant> plants;

Plant tomate = {"Tomate", 1, 2};
Plant pepino = {"Pepino", 2, 3};
Plant rucula = {"Rucula", 3, 4};
Plant choclo = {"Choclo", 4, 5};

plants.add(tomate);
plants.add(pepino);
plants.add(rucula);
plants.add(choclo);

#define BUTTON_UP 26
#define BUTTON_DOWN 25
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
int returnState = HOME;

bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

bool selected = 0;
int printIndex = 0;
int chosenPlant = 0;

int seg = 0;
int ms = 0;
int segPrint = 0;
#define T_PRINT 1

void ScrollDown(void);
void ScrollUp(void);

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
  lcd.setCursor(0, 0);
  lcd.print("*");
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print(plants.get(printIndex.name));
  lcd.setCursor(1, 1);
  lcd.print(plants.get((printIndex + 1).name));
}

void loop() {

  ReadButtonDown = digitalRead(BUTTON_DOWN);
  ReadButtonUp = digitalRead(BUTTON_UP);
  ReadButtonOk = digitalRead(BUTTON_OK);
  ReadButtonWater = digitalRead(BUTTON_WATER);

  switch (actualState) {
    case HOME:
      returnState = HOME;
      lcd.setCursor(1, 0);
      lcd.print(plants.get(printIndex));
      lcd.setCursor(1, 1);
      lcd.print(plants.get(printIndex + 1));

      if (ReadButtonDown == PUSHED) {
        actualState = DOWN;
      }

      if (ReadButtonUp == PUSHED) {
        actualState = UP;
      }

      if (ReadButtonOk == PUSHED) {
        actualState = PRINT;
        lcd.clear();
        chosenPlant = printIndex;
        if (selected == 1) {
          chosenPlant++;
        }
        Serial.println(printIndex);
      }

      break;

    case DOWN:
      if (ReadButtonDown == N_PUSHED) {
        ScrollDown();
        actualState = returnState;
      }
      break;

    case UP:
      if (ReadButtonUp == N_PUSHED) {
        Serial.println("go up");
        ScrollUp();
        actualState = returnState;
      }
      break;

    case PRINT:
    Serial.println(ReadButtonUp);
      if (ReadButtonUp == N_PUSHED) {
        returnState = PRINT;
        lcd.setCursor(1, 0);
        lcd.print("Planta:");
        lcd.setCursor(8, 0);
        lcd.print(plants.get(chosenPlant));
        lcd.setCursor(1, 1);
        lcd.print("Temperatura:");
        lcd.setCursor(13, 1);
        lcd.print(temperature.get(chosenPlant));
      }
      if (ReadButtonDown == PUSHED) {
          actualState = DOWN;
        }

        if (ReadButtonUp == PUSHED) {
          actualState = UP;
          Serial.println("should up");
        }
      break;
  }
}

void ScrollDown() {
  if (selected == 1) {
    Serial.print("1 --> ");
    selected = 0;
    lcd.clear();
    Serial.println(selected);
    lcd.setCursor(0, 0);
    lcd.print("*");
    lcd.setCursor(0, 1);
    lcd.print("");
    if (plants.getSize() == (printIndex + 2)) {
      Serial.print(printIndex);
      Serial.print(" | ");
      Serial.println(plants.getSize());
      printIndex = 0;
    } else {
      printIndex = printIndex + 1;
    }
    return;
  } else {
    Serial.print("0 --> ");
    selected = 1;
    lcd.clear();
    Serial.println(selected);
    lcd.setCursor(0, 0);
    lcd.print("");
    lcd.setCursor(0, 1);
    lcd.print("*");
    return;
  }
}

void ScrollUp() {
  Serial.println("up");
  if (selected == 1) {
    Serial.print("1 --> ");
    selected = 0;
    lcd.clear();
    Serial.println(selected);
    lcd.setCursor(0, 0);
    lcd.print("*");
    lcd.setCursor(0, 1);
    lcd.print("");
    return;
  } else if (selected == 0) {
    Serial.print("0 --> ");
    selected = 1;
    lcd.clear();
    Serial.println(selected);
    lcd.setCursor(0, 0);
    lcd.print("");
    lcd.setCursor(0, 1);
    lcd.print("*");
    if (printIndex == 0) {
      printIndex = plants.getSize() - 2;
    } else {
      printIndex = printIndex - 1;
    }
  }
  return;
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
