#include <List.hpp>
#include <LiquidCrystal_I2C.h>  //21: SDA || 22: SLC

struct Plant {
  int moduleID;
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

int plantZone;

struct PlantReads {
  int tempRead;
  int humRead;
  int lastRead;
  int lastWater;
};

List<Plant> plants(true);             // HECHA: Lista de objetos de planta con la info ya lista para el usuario
List<IdealValues> idealValues(true);  // HECHA: Lista de valores ideales según tipo de planta
List<PlantReads> plantReads(true);    // HECHA: Lista de lecturas según planta: valores no procesados
List<String> plantPrint(true);        // HECHA: Lista de parámetros a imprimir en la pantalla
List<String> plantInfo(true);         // HECHA: Lista de info de la planta elegida a imprimir en la pantalla

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

int readsMade = 0;
#define MIN_READS 10

// Pines
#define BUTTON_UP 26
#define BUTTON_DOWN 27
#define BUTTON_OK 33
#define BUTTON_WATER 25

#define LED_WATER 4
#define LED_LOADING 15
#define LED_ON 5

#define PUSHED LOW
#define N_PUSHED !PUSHED

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
#define GO_UP -1
#define GO_DOWN 1

int actualState = PROCESS_LECTURES;
int returnState = HOME;
int printDetail;

bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

bool selected = MIN_OPTIONS;
int printIndex = 0;
unsigned long lastPrint = 0;
#define T_PRINT 1000
unsigned long now = 0;

#define I2C_DIRECTION 0x27
#define SCREEN_LENGHT 16
#define SCREEN_HEIGHT 2

LiquidCrystal_I2C lcd(I2C_DIRECTION, SCREEN_LENGHT, SCREEN_HEIGHT);

// Declaración de funciones
int ScrollDown(int, int);
int ScrollUp(int, int);
void PrintPlantsNames(void);
void PrintPlantsDetails(void);
String FilterReads(int, int, int, int, int, int);

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

  idealValues.add(tomateValues);
  idealValues.add(pepinoValues);
  idealValues.add(ruculaValues);
  idealValues.add(chocloValues);

  plantReads.add(tomateReads);
  plantReads.add(pepinoReads);
  plantReads.add(ruculaReads);
  plantReads.add(chocloReads);

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
  //Serial.println(actualState);

  switch (actualState) {
    case HOME:
      int chosenPlant;
      if (ReadButtonDown == PUSHED) {
        //Serial.println("pre down");
        while (ReadButtonDown == PUSHED) {
          while (readsMade < MIN_READS) {
            //Serial.println("wait");
            ReadButtonDown = digitalRead(BUTTON_DOWN);
            readsMade = readsMade + 1;
          }
          ReadButtonDown = digitalRead(BUTTON_DOWN);
        }
        printIndex = ScrollDown(plants.getSize(), printIndex);
        readsMade = 0;
      }

      if (ReadButtonUp == PUSHED) {
        //Serial.println("pre up");
        /*
        while ((ReadButtonUp == PUSHED) || (readsMade < MIN_READS)) {
          //Serial.println("wait");
          ReadButtonUp = digitalRead(BUTTON_UP);
          readsMade = readsMade + 1;
        }
        printIndex = ScrollUp(plants.getSize(), printIndex);
        //Serial.println("up");
        readsMade = 0;*/
        actualState = UP;
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
        printPlantsDetails();
        selected = MIN_OPTIONS;
        printDetail = MIN_OPTIONS;
      } else if (now - lastPrint >= T_PRINT) {
        printPlantsNames();
        lastPrint = millis();
      }
      break;

    case UP:
      if (ReadButtonDown == N_PUSHED) {
        printIndex = ScrollUp(plants.getSize(), printIndex);
        actualState = HOME;
      }
      break;
    case PRINT:
      Serial.println(selected);
      if (now - lastPrint >= T_PRINT) {
        printPlantsDetails();
        lastPrint = millis();
      }

      if (ReadButtonDown == PUSHED) {
        Serial.println("pre down");
        while (ReadButtonDown == PUSHED) {
          Serial.println("wait");
          ReadButtonDown = digitalRead(BUTTON_DOWN);
        }
        printDetail = ScrollDown(plantPrint.getSize(), printDetail);
        Serial.println("down");
      }

      if (ReadButtonUp == PUSHED) {
        Serial.println("pre up");
        while (ReadButtonUp == PUSHED) {
          Serial.println("wait");
          ReadButtonUp = digitalRead(BUTTON_UP);
        }
        printDetail = ScrollUp(plantPrint.getSize(), printDetail);
        Serial.println("UP");
      }
      break;

    case PROCESS_LECTURES:
      for (int process = 0; process <= plants.getSize(); process++) {
        Serial.println(process);
        plants.get(process).tempStatus = filterReads(plantReads.get(process).tempRead, MIN_TEMP, MAX_TEMP, idealValues.get(process).tempIdeal, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
        plants.get(process).humStatus = filterReads(plantReads.get(process).humRead, MIN_HUM, MAX_HUM, idealValues.get(process).humIdeal, HUM_OK_RANGE, HUM_EXCEED_RANGE);
      }
      actualState = HOME;
      break;

    case BACK_HOME:
      if ((ReadButtonWater == N_PUSHED) && (ReadButtonOk == N_PUSHED) && (ReadButtonDown == N_PUSHED) && (ReadButtonUp == N_PUSHED)) {
        actualState = HOME;
      }
      break;
  }
}
//CONVERTIR EN DOWNNN
int ScrollDown(int maxSize, int index) {
  if (selected != MAX_OPTIONS) {
    selected = MIN_OPTIONS;
    index += GO_DOWN;
    //Serial.println("sel");
    Serial.println("down");
  } else {
    Serial.println("down");
    selected = MIN_OPTIONS;
    if (index == maxSize - MAX_OPTIONS - 1) {
      index = 0;
      //Serial.println("min");
    } else {
      index += GO_DOWN;
      //Serial.println("down");
    }
  }
  return index;
}

int ScrollUp(int maxSize, int index) {
  if (selected != MIN_OPTIONS) {
    selected = selected + GO_UP;
    //Serial.println("sel");
    Serial.println("up");
  } else {
    Serial.println("up");
    selected = MAX_OPTIONS;
    if (index == 0) {
      index = maxSize - MAX_OPTIONS - 1;
      //Serial.println("max");
    } else {
      index += GO_UP;
      //Serial.println("up");
    }
  }
  return index;
}

void printPlantsNames(void) {
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.clear();
  lcd.setCursor(0, selected);
  lcd.print("*");
  for (int printed = 0; printed <= MAX_OPTIONS; printed++) {
    String actualParameter = plantPrint.get(printIndex + printed);
    lcd.setCursor(1, printed);
    lcd.print(plants.get(printIndex + printed).plantName);
  }
}

void printPlantsDetails() {
  lcd.clear();
  lcd.setCursor(0, selected);
  lcd.print("*");
  for (int infoShown = 0; infoShown <= MAX_OPTIONS; infoShown++) {
    lcd.setCursor(1, infoShown);
    lcd.print(plantPrint.get(printIndex + infoShown));
    lcd.setCursor(9, infoShown);
    lcd.print(plantInfo.get(printIndex + infoShown));
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
  return status;
}