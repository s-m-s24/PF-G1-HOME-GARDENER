#include <List.hpp>
#include <LiquidCrystal_I2C.h>  //21: SDA || 22: SLC

struct Plant {
  int PlantID;
  String plantName;
  String tempStatus;
  String humStatus;
  String lastRead;     //ÚLTIMA VEZ QUE LEÍ
  String lastWater;    //ÚLTIMA VEZ QUE REGUÉ
  String timeWatered;  //TIEMPO QUE REGUÉ
};

struct IdealValues {
  int tempIdeal;
  int humIdeal;
};

struct PlantReads {
  int tempRead;
  int humRead;
  int lastRead;

  int lastWater;
};

List<Plant> plants(true);
List<String> plantPrint(true);
List<String> plantInfo(true);

Plant tomate = { 1, "Tomate", "", "", "1100", "202", "3" };
Plant pepino = { 2, "Pepino", "", "", "2000", "18", "1" };
Plant rucula = { 3, "Rucula", "", "", "400", "100", "0" };
Plant choclo = { 4, "Choclo", "", "", "808", "750", "5" };

IdealValues tomateValues = { 15, 1000 };
IdealValues pepinoValues = { 20, 1500 };
IdealValues ruculaValues = { 5, 800 };
IdealValues chocloValues = { 25, 2000 };

PlantReads tomateReads = { 20, 1100, 0, 0 };
PlantReads pepinoReads = { 10, 1000, 0, 0 };
PlantReads ruculaReads = { 20, 200, 0, 0 };
PlantReads chocloReads = { 80, 1800, 0, 0 };

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

#define TEMP_IDEAL 20
#define TEMP_OK_RANGE 5
#define TEMP_EXCEED_RANGE 10
#define MIN_TEMP -20
#define MAX_TEMP 40

#define HUM_IDEAL 2050
#define HUM_OK_RANGE 300
#define HUM_EXCEED_RANGE 800
#define MIN_HUM 500
#define MAX_HUM 3800

bool mustWater = false;

// Estados
#define HOME 1
#define DOWN 2
#define UP 3
#define PRINT 4
#define PROCESS_LECTURES 5
#define BACK_HOME 6

#define MAX_OPTIONS 1
#define MIN_OPTIONS 0

int actualState = PROCESS_LECTURES;
int returnState = HOME;
int printDetail;

#define PRINT_NAME_TEMP 0
#define PRINT_TEMP_HUM 1
#define PRINT_HUM_READS 2
#define PRINT_READS_WATER 3
#define PRINT_WATER_WATERED 4


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
String filterReads(int, int, int, int, int, int);

void setup() {
  Serial.begin(9600);

  plants.add(tomate);
  plants.add(pepino);
  plants.add(rucula);
  plants.add(choclo);

  plantPrint.add("Nombre");
  plantPrint.add("Temp");
  plantPrint.add("Hum");
  plantPrint.add("Leído");
  plantPrint.add("Regado");
  plantPrint.add("T riego");

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
        Serial.println("pre down");
        while (ReadButtonDown == PUSHED) {
          Serial.println("wait");
          ReadButtonDown = digitalRead(BUTTON_DOWN);
        }
        ScrollDown();
        Serial.println("down");
      }

      if (ReadButtonUp == PUSHED) {
        Serial.println("pre up");
        while (ReadButtonUp == PUSHED) {
          Serial.println("wait");
          ReadButtonUp = digitalRead(BUTTON_UP);
        }
        ScrollUp();
        Serial.println("up");
      }

      if (ReadButtonOk == PUSHED) {
        while (ReadButtonOk == PUSHED) {
          Serial.println("wait");
          ReadButtonOk = digitalRead(BUTTON_OK);
        }
        actualState = PRINT;
        chosenPlant = printIndex + selected;
        plantInfo.add(plants.get(chosenPlant).plantName);
        plantInfo.add(plants.get(chosenPlant).tempStatus);
        plantInfo.add(plants.get(chosenPlant).humStatus);
        plantInfo.add(plants.get(chosenPlant).lastRead);
        plantInfo.add(plants.get(chosenPlant).lastWater);
        plantInfo.add(plants.get(chosenPlant).timeWatered);
        printPlantsDetails(chosenPlant);
        selected = 0;
        printDetail = MIN_OPTIONS;
      } else if (now - lastPrint >= T_PRINT) {
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
      Serial.println(selected);
      if (now - lastPrint >= T_PRINT) {
        printPlantsDetails(chosenPlant);
        lastPrint = millis();
      }

      if (ReadButtonDown == PUSHED) {
        Serial.println("pre down");
        while (ReadButtonDown == PUSHED) {
          Serial.println("wait");
          ReadButtonDown = digitalRead(BUTTON_DOWN);
        }
        if (selected != MAX_OPTIONS) {
          selected = selected + 1;
        } else {
          selected = 0;
          if (printDetail == PRINT_WATER_WATERED) {
            printDetail = PRINT_NAME_TEMP;
          } else {
            printDetail = printDetail + 1;
          }
        }
        Serial.println("down");
      }

      if (ReadButtonUp == PUSHED) {
        Serial.println("pre up");
        while (ReadButtonUp == PUSHED) {
          Serial.println("wait");
          ReadButtonUp = digitalRead(BUTTON_UP);
        }
        if (selected != MIN_OPTIONS) {
          selected = selected - 1;
        } else {
          selected = 1;
          if (printDetail == PRINT_NAME_TEMP) {
            printDetail = PRINT_WATER_WATERED;

          } else {
            printDetail = printDetail - 1;
          }
        }
        Serial.println("UP");
      }
      /*
      if (ReadButtonWater == PUSHED) {
        actualState = BACK_HOME;
      }
      */

      break;

    case PROCESS_LECTURES:

      // sistematizar
      tomate.tempStatus = filterReads(tomateReads.tempRead, MIN_TEMP, MAX_TEMP, tomateValues.tempIdeal, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
      tomate.humStatus = filterReads(tomateReads.humRead, MIN_HUM, MAX_HUM, tomateValues.humIdeal, HUM_OK_RANGE, HUM_EXCEED_RANGE);

      pepino.tempStatus = filterReads(pepinoReads.tempRead, MIN_TEMP, MAX_TEMP, pepinoValues.tempIdeal, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
      pepino.humStatus = filterReads(pepinoReads.humRead, MIN_HUM, MAX_HUM, pepinoValues.humIdeal, HUM_OK_RANGE, HUM_EXCEED_RANGE);

      rucula.tempStatus = filterReads(ruculaReads.tempRead, MIN_TEMP, MAX_TEMP, ruculaValues.tempIdeal, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
      rucula.humStatus = filterReads(ruculaReads.humRead, MIN_HUM, MAX_HUM, ruculaValues.humIdeal, HUM_OK_RANGE, HUM_EXCEED_RANGE);

      choclo.tempStatus = filterReads(chocloReads.tempRead, MIN_TEMP, MAX_TEMP, chocloValues.tempIdeal, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
      choclo.humStatus = filterReads(chocloReads.humRead, MIN_HUM, MAX_HUM, chocloValues.humIdeal, HUM_OK_RANGE, HUM_EXCEED_RANGE);
      actualState = HOME;
      break;

    case BACK_HOME:
      if ((ReadButtonWater == N_PUSHED) && (ReadButtonOk == N_PUSHED) && (ReadButtonDown == N_PUSHED) && (ReadButtonUp == N_PUSHED)) {
        actualState = HOME;
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
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.clear();
  lcd.setCursor(0, selected);
  lcd.print("*");
  for (int printed = 0; printed <= MAX_OPTIONS; printed++) {
    lcd.setCursor(1, printed);
    lcd.print(plants.get(printIndex + printed).plantName);
  }
}

void printPlantsDetails(int chosenPlant) {
  lcd.clear();
  lcd.setCursor(0, selected);
  lcd.print("*");
  if (printDetail == MIN_OPTIONS) {
    lcd.setCursor(1, 0);
    lcd.print("Planta:");
    lcd.setCursor(8, 0);
    lcd.print(plants.get(chosenPlant).plantName);
    lcd.setCursor(1, 1);
    lcd.print("T:");
    lcd.setCursor(3, 1);
    lcd.print(plants.get(chosenPlant).tempStatus);
  }
  if (printDetail == MAX_OPTIONS) {
    lcd.setCursor(1, 0);
    lcd.print("T:");
    lcd.setCursor(3, 0);
    lcd.print(plants.get(chosenPlant).tempStatus);
    lcd.setCursor(1, 1);
    lcd.print("Humedad:");
    lcd.setCursor(9, 1);
    lcd.print(plants.get(chosenPlant).humStatus);
  }
}


String filterReads(int lecture, int MIN, int MAX, int IDEAL, int OK_RANGE, int EXCEED_RANGE) {
  /*
  requires: lecture, minimum, maximum, ideal value, permited range, exceeding range
  */
  mustWater = false;

  String status = "";
  if ((lecture <= MIN) || (lecture >= MAX)) {
    status = "PROBLEMA!";

  } else {
    if ((lecture <= IDEAL + OK_RANGE) && (lecture >= IDEAL - OK_RANGE)) {
      status = "IDEAL";
      mustWater = false;
    } else if ((lecture <= IDEAL + EXCEED_RANGE) && (lecture >= IDEAL + OK_RANGE)) {
      status = "ALTO";
      mustWater = false;
    } else if ((lecture >= IDEAL - EXCEED_RANGE) && (lecture <= IDEAL - OK_RANGE)) {
      status = "BAJ0";
      mustWater = true;
    } else if (lecture <= IDEAL - EXCEED_RANGE) {
      status = "MUY BAJ0";
      mustWater = true;
    } else if (lecture >= IDEAL + EXCEED_RANGE) {
      status = "MUY ALT0";
      mustWater = false;
    }
  }
  //Serial.print(" | ");
  //Serial.println(status);
  return status;
}