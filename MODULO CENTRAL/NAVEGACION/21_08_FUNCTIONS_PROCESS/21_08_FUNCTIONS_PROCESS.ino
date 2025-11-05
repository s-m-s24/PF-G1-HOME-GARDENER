#include <List.hpp>
#include <LiquidCrystal_I2C.h>  //21: SDA || 22: SLC

struct Plant {
  int moduleID;
  String plantName;
  String tempStatus;
  String humStatus;
  String lastRead;  //ÚLTIMA VEZ QUE LEÍ
  String lastWater;
  String timeWatered;  //TIEMPO QUE REGUÉ
};

struct IdealValues {
  int tempIdeal;
  int humIdeal;
  int location;
  bool hasTempSensor;
};

struct PlantReads {
  int tempRead;
  int humRead;   //TIEMPO QUE REGUÉ
  int lastRead;  //ÚLTIMA VEZ QUE LEÍ
  int lastWater;
};

List<Plant> plants(true);             // HECHA: Lista de objetos de planta con la info ya lista para el usuario
List<IdealValues> idealValues(true);  // HECHA: Lista de valores ideales según tipo de planta
List<PlantReads> plantReads(true);    // HECHA: Lista de lecturas según planta: valores no procesados
List<String> plantPrint(true);        // HECHA: Lista de parámetros a imprimir en la pantalla
List<String> plantInfo(false);        // HECHA: Lista de info de la planta elegida a imprimir en la pantalla

Plant tomate = { 1, "Tomate", "MALO", "BUENO", "1100", "202", "3" };
Plant pepino = { 2, "Pepino", "BIEN", "MUY BUENO", "2000", "18", "1" };
Plant rucula = { 3, "Rucula", "CASI", "MUY MAL", "400", "100", "0" };
Plant choclo = { 4, "Choclo", "MALO", "BIEN", "808", "750", "5" };
IdealValues tomateValues = { 15, 1000, 3, true };
IdealValues pepinoValues = { 20, 1500, 4, true };
IdealValues ruculaValues = { 5, 800, 3, false };
IdealValues chocloValues = { 25, 2000, 3, false };

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

int actualState = HOME;
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
unsigned long lastRead = 0;
#define T_READ 5

//BOTÓN 1:
void antiReboteBoton1(void);
//Boton
#define ESPERA_BOT1 1
#define CONFIRMACION_BOT1 2
#define LIBERACION_BOT1 3
bool flagBot1 = N_PUSHED;  //
int estadoMaquinaBoton1 = ESPERA_BOT1;

//BOTÓN 1:
void antiReboteBoton2(void);
//Boton
#define ESPERA_BOT2 1
#define CONFIRMACION_BOT2 2
#define LIBERACION_BOT2 3
bool flagBot2 = N_PUSHED;  //
int estadoMaquinaBoton2 = ESPERA_BOT2;

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

  plantPrint.add(String("Nombre"));
  plantPrint.add(String("Temp"));
  plantPrint.add(String("Hum"));
  plantPrint.add(String("Leído"));
  plantPrint.add(String("Regado"));
  plantPrint.add(String("T riego"));

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
  //printPlantsNames();
  lcd.setCursor(0, 1);
  lcd.print("*");
  lcd.setCursor(0, 0);
  lcd.print("-");
}

void loop() {
  //Serial.println("");

  now = millis();
  //Serial.println(actualState);

  switch (actualState) {
    case HOME:
      ReadButtonDown = digitalRead(BUTTON_DOWN);
      ReadButtonUp = digitalRead(BUTTON_UP);
      ReadButtonOk = digitalRead(BUTTON_OK);
      ReadButtonWater = digitalRead(BUTTON_WATER);
      Serial.print("UP:");
      Serial.print(ReadButtonUp);
      Serial.print(" | DOWN:");
      Serial.print(ReadButtonDown);
      Serial.print(" | OK:");
      Serial.print(ReadButtonOk);
      Serial.print(" | SIZE:");
      Serial.println(plants.getSize());

      int chosenPlant;
      antiReboteBoton1();
      antiReboteBoton2();

      if (flagBot2 == PUSHED) {
        printIndex = ScrollUp(plants.getSize(), printIndex);
        flagBot2 = N_PUSHED;
      }
      if (flagBot1 == PUSHED) {
        printIndex = ScrollDown(plants.getSize(), printIndex);
        flagBot1 = N_PUSHED;
      }

      if (ReadButtonOk == PUSHED) {
        while (ReadButtonOk == PUSHED) {
          Serial.println("wait");
          ReadButtonOk = digitalRead(BUTTON_OK);
        }
        chosenPlant = printIndex + selected;
        plantInfo.clear();
        plantInfo.add(String(plants.get(chosenPlant).plantName));
        Serial.print("NAME: ");
        //Serial.println(plants.get(chosenPlant).plantName);
        Serial.println(plantInfo.get(0));

        plantInfo.add(String(plants.get(chosenPlant).tempStatus));
        Serial.print("TEMP: ");
        //Serial.println(plants.get(chosenPlant).tempStatus);
        Serial.println(plantInfo.get(1));

        plantInfo.add(String(plants.get(chosenPlant).humStatus));
        Serial.print("HUM: ");
        //Serial.println(plants.get(chosenPlant).humStatus);
        Serial.println(plantInfo.get(2));

        plantInfo.add(String(plants.get(chosenPlant).lastRead));
        Serial.print("READ: ");
        //Serial.println(plants.get(chosenPlant).lastRead);
        Serial.println(plantInfo.get(3));

        plantInfo.add(String(plants.get(chosenPlant).lastWater));
        Serial.print("WATER: ");
        //Serial.println(plants.get(chosenPlant).lastWater);
        Serial.println(plantInfo.get(4));

        plantInfo.add(String(plants.get(chosenPlant).timeWatered));
        Serial.print("TIME: ");
        //Serial.println(plants.get(chosenPlant).timeWatered);
        Serial.println(plantInfo.get(5));

        Serial.print("SIZE:");
        Serial.println(plantInfo.getSize());
        Serial.println(plantInfo.get(4));

        selected = MIN_OPTIONS;
        printDetail = MIN_OPTIONS;
        actualState = PRINT;
        printIndex = 0;
      } else if (now - lastPrint >= T_PRINT) {
        printPlantsNames();
        lastPrint = millis();
      }
      break;

    case UP:
      if ((ReadButtonUp == N_PUSHED) && (lastRead - now >= T_READ)) {
        Serial.print("Gone --> ");
        printIndex = ScrollUp(plants.getSize(), printIndex);
        actualState = returnState;
      }
      break;
    case DOWN:
      if ((ReadButtonDown == N_PUSHED) && (lastRead - now >= T_READ)) {
        Serial.print("Gone --> ");
        printIndex = ScrollDown(plants.getSize(), printIndex);
        actualState = returnState;
      }
      break;
    case PRINT:
      //Serial.println(selected);
      if (now - lastPrint >= T_READ) {
        printPlantsDetails();
        lastPrint = millis();
      }
      if (ReadButtonDown == PUSHED) {
        antiReboteBoton1();
      }

      if (ReadButtonUp == PUSHED) {
        antiReboteBoton2();
      }

      if (flagBot2 == PUSHED) {
        printDetail = ScrollUp(plantPrint.getSize(), printDetail);
        flagBot2 = N_PUSHED;
      }
      if (flagBot1 == PUSHED) {
        printDetail = ScrollDown(plantPrint.getSize(), printDetail);
        flagBot1 = N_PUSHED;
      }
      break;

    case PROCESS_LECTURES:
      Serial.println("procesando...");
      int tempIndex;
      for (int process = 0; process < plants.getSize(); process++) {
        Serial.print(plants.get(process).plantName);
        if (idealValues.get(process).hasTempSensor == false) {
          for (int plantsChecked = 0; plantsChecked < idealValues.getSize(); plantsChecked++) {
            if ((idealValues.get(plantsChecked).location) == (idealValues.get(process).location)) {
              if (idealValues.get(plantsChecked).hasTempSensor == true) {
                tempIndex = plantsChecked;
                Serial.print(plantsChecked);
                Serial.print(" |cambio t: ");
                Serial.print(plantReads.get(tempIndex).tempRead);
                Serial.print(" | TEMP: ");
                plants.get(process).tempStatus = filterReads(plantReads.get(tempIndex).tempRead, MIN_TEMP, MAX_TEMP, idealValues.get(process).tempIdeal, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
              }
            }
          }
        } else {
          Serial.print(" | TEMP: ");
          plants.get(process).tempStatus = filterReads(plantReads.get(process).tempRead, MIN_TEMP, MAX_TEMP, idealValues.get(process).tempIdeal, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
        }
        Serial.print(" | HUM: ");
        plants.get(process).humStatus = filterReads(plantReads.get(process).humRead, MIN_HUM, MAX_HUM, idealValues.get(process).humIdeal, HUM_OK_RANGE, HUM_EXCEED_RANGE);
        Serial.println("");
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

int ScrollDown(int maxSize, int index) {
  //A FUTURO: FUSIONAR DOWN Y UP --> LOS VALORES Y ACCIONES DEPENDEN DEL BOTON ELEGIDO?????????????????????????????????????????????????
  Serial.print("Done: ");
  if (selected != MAX_OPTIONS) {
    selected = selected + GO_DOWN;
    Serial.println("sel");
    return index;
  } else {
    //Serial.println("down");
    selected = MIN_OPTIONS;
    if (index == maxSize - MAX_OPTIONS - 1) {
      index = 0;
      Serial.println("min");
      return index;
    } else {
      index += GO_DOWN;
      Serial.println("down");
      return index;
    }
  }
}

int ScrollUp(int maxSize, int index) {
  Serial.print("Done: ");
  if (selected != MIN_OPTIONS) {
    selected = selected + GO_UP;
    Serial.println("sel");
    //Serial.println("up");
    return index;
  } else {
    //Serial.println("up");
    selected = MAX_OPTIONS;
    if (index == 0) {
      index = maxSize - MAX_OPTIONS - 1;
      Serial.println("max");
      return index;
    } else {
      index += GO_UP;
      Serial.println("up");
      return index;
    }
  }
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
    int idx = printDetail + infoShown;
    if (idx < plantPrint.getSize() && idx < plantInfo.getSize()) {
      lcd.setCursor(1, infoShown);
      lcd.print(plantPrint.get(idx));
      lcd.setCursor(9, infoShown);
      lcd.print(plantInfo.get(idx));
    }
  }
  return;
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
  Serial.print(status);
  return status;
}

void antiReboteBoton1(void) {
  bool estadoBoton1 = digitalRead(BUTTON_DOWN);
  //Serial.print(estadoBoton1);
  //Serial.print(" | ");
  //Serial.println("inicio");
  switch (estadoMaquinaBoton1) {
    unsigned long lastReadBot;
    case ESPERA_BOT1:
      //Serial.println("espera");
      if (estadoBoton1 == PUSHED) {
        estadoMaquinaBoton1 = CONFIRMACION_BOT1;
        //Serial.println("confirmo");
      }
      break;

    case CONFIRMACION_BOT1:
      //Serial.println("confirmo");
      //Serial.print(lastReadBot - now);
      //Serial.print(" | ");
      //Serial.println(estadoBoton1);
      if (((lastReadBot - now) >= T_READ) && (estadoBoton1 == N_PUSHED)) {
        estadoMaquinaBoton1 = ESPERA_BOT1;
        //Serial.println("espero");
      }
      if (((lastReadBot - now) >= T_READ) && (estadoBoton1 == PUSHED)) {
        estadoMaquinaBoton1 = LIBERACION_BOT1;
        //Serial.println("libero");
        //flag de Flanco boton PUSHED.
      }
      break;

    case LIBERACION_BOT1:
      //Serial.println("lib:");
      if (estadoBoton1 == N_PUSHED) {
        flagBot1 = PUSHED;
        estadoMaquinaBoton1 = ESPERA_BOT1;
        //Serial.println("PUSHED");
      }
      break;
  }
  return;
}

void antiReboteBoton2(void) {
  //PONER LOS FLAGS EN N_PUSHED CUANDO LLAMO AL SCROLL
  //VER PQ CARAJOS NO ANDAN LOS ANTIRREBOTS
  //CONSEGUIR QUE IMPRIMA BIEN LOS DETALLES
  bool estadoBoton2 = digitalRead(BUTTON_UP);
  //Serial.print(estadoBoton2);
  //Serial.print(" | ");
  switch (estadoMaquinaBoton2) {
    unsigned long lastReadBot;
    case ESPERA_BOT2:
      //Serial.println("espera");
      if (estadoBoton2 == PUSHED) {
        estadoMaquinaBoton2 = CONFIRMACION_BOT2;
        //Serial.println("confirmo");
      }
      break;

    case CONFIRMACION_BOT2:
      //Serial.println("confirmo");
      //Serial.print(lastReadBot - now);
      //Serial.print(" | ");
      //Serial.println(estadoBoton2);
      estadoBoton2 = digitalRead(BUTTON_UP);
      if (((lastReadBot - now) >= T_READ) && (estadoBoton2 == N_PUSHED)) {
        estadoMaquinaBoton2 = ESPERA_BOT2;
        //Serial.println("espero");
      }
      if (((lastReadBot - now) >= T_READ) && (estadoBoton2 == PUSHED)) {
        estadoMaquinaBoton2 = LIBERACION_BOT2;
        //Serial.println("libero");
        //flag de Flanco boton PUSHED.
      }
      //Serial.println(estadoMaquinaBoton2);
      break;

    case LIBERACION_BOT2:
      //Serial.println("lib:");
      if (estadoBoton2 == N_PUSHED) {
        flagBot2 = PUSHED;
        estadoMaquinaBoton2 = ESPERA_BOT2;
        //Serial.println(" PUSHED");
      }
      break;
  }
  return;
}
