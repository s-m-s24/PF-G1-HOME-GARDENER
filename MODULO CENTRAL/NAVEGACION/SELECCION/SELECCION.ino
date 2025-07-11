#include <List.hpp>
#include <LiquidCrystal_I2C.h>

struct Plant {
  String plantName;
  int temp;
  int humid;
};

List<Plant> plants(true);

Plant tomate = { "Tomate", 12, 2 };
Plant pepino = { "Pepino", 23, 3 };
Plant rucula = { "Rucula", 34, 4 };
Plant choclo = { "Choclo", 45, 5 };

// Pines
#define BUTTON_UP 26
#define BUTTON_DOWN 27
#define BUTTON_OK 33
#define BUTTON_WATER 25

#define LED_WATER 4
#define LED_LOADING 15
#define LED_ON 5

#define PUSHED LOW
#define N_PUSHED HIGH

// Estados
#define HOME 1
#define DOWN 2
#define UP 3
#define PRINT 4

#define MAX_OPTIONS 1
#define MIN_OPTIONS 0

int actualState = HOME;
int returnState = HOME;

bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

bool selected = 0;
int printIndex = 0;
unsigned long lastPrint = 0;
#define T_PRINT 1000
unsigned long now = 0;


LiquidCrystal_I2C lcd(0x27, 16, 2);

// Declaración de funciones
void ScrollDown(void);
void ScrollUp(void);
void printPlantsNames(void);
void printPlantsDetails(int);

void setup() {
  Serial.begin(115200);

  plants.add(tomate);
  plants.add(pepino);
  plants.add(rucula);
  plants.add(choclo);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);
  pinMode(BUTTON_WATER, INPUT_PULLUP);

  pinMode(LED_WATER, OUTPUT);
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_LOADING, OUTPUT);

  lcd.init();
  lcd.backlight();
  printPlantsNames();
}

void loop() {
  ReadButtonDown = digitalRead(BUTTON_DOWN);
  ReadButtonUp = digitalRead(BUTTON_UP);
  ReadButtonOk = digitalRead(BUTTON_OK);
  ReadButtonWater = digitalRead(BUTTON_WATER);

  now = millis();

  static int chosenPlant;

  switch (actualState) {
    case HOME:
      if (ReadButtonDown == PUSHED) {
        actualState = DOWN;
        Serial.println("down");
      }

      if (ReadButtonUp == PUSHED) {
        actualState = UP;
        Serial.println("up");
      }

      if (ReadButtonOk == PUSHED) {
        actualState = PRINT;
        chosenPlant = printIndex + selected;
        Serial.print("ok: ");
        Serial.println(chosenPlant);
        printPlantsDetails(chosenPlant);
        selected = MIN_OPTIONS;
        returnState = PRINT;
      } 
      else if (now - lastPrint >= T_PRINT) {
        printPlantsNames();
        lastPrint = millis();
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
        ScrollUp();
        actualState = returnState;
      }
      break;

    case PRINT:
      if (now - lastPrint >= T_PRINT) {
        printPlantsDetails(chosenPlant);
        lastPrint = millis();
      }

      if (ReadButtonDown == PUSHED) {
        actualState = DOWN;
      }

      if (ReadButtonUp == PUSHED) {
        actualState = UP;
      }

      break;
  }
}

void ScrollDown() {
  if (selected == MAX_OPTIONS) {
    selected = 0;
    if (plants.getSize() == (printIndex + 2)) {
      printIndex = 0;
    } else {
      printIndex += 1;
    }
  } else {
    selected += 1;
  }
}

void ScrollUp() {
  if (selected != MIN_OPTIONS) {
    selected -= 1;
  } else {
    selected = 1;
    if (printIndex == 0) {
      printIndex = plants.getSize() - 2;
    } else {
      printIndex -= 1;
    }
  }
}

void printPlantsNames(void) {
  lcd.clear();
  lcd.setCursor(0, selected);
  lcd.print("*");
  for (int printed = 0; printed <= MAX_OPTIONS; printed++) {
    lcd.setCursor(1, printed);
    lcd.print(plants.get(printIndex + printed).plantName);
  }
}

void printPlantsDetails(int chosenPlant) {
  bool orderPrint = !selected;
  lcd.clear();
  lcd.setCursor(0, selected);
  lcd.print("*");
  if (selected == MIN_OPTIONS){
    lcd.setCursor(1, 0);
  lcd.print("Planta:");
  lcd.setCursor(8, 0);
  lcd.print(plants.get(chosenPlant).plantName);
  }  
  lcd.setCursor(1, orderPrint);
  lcd.print("Temperatura:");
  lcd.setCursor(13, orderPrint);
  lcd.print(plants.get(chosenPlant).temp);
  if (selected == MAX_OPTIONS){
   lcd.setCursor(1, 1);
  lcd.print("Humedad:");
  lcd.setCursor(9, 1);
  lcd.print(plants.get(chosenPlant).humid); 
  }
}
