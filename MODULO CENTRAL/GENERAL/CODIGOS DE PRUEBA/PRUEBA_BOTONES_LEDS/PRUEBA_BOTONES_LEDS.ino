//#include <LiquidCrystal_I2C.h> //21: SDA || 22: SLC

#define BUTTON_UP 26
#define BUTTON_DOWN 27
#define BUTTON_OK 33
#define BUTTON_WATER 25

#define LED_WATER 4
#define LED_LOADING 15
#define LED_ON 5

#define PUSHED LOW
#define N_PUSHED HIGH

bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

//LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_UP,INPUT_PULLUP);
  pinMode(BUTTON_DOWN,INPUT_PULLUP);
  pinMode(BUTTON_OK,INPUT_PULLUP);
  pinMode(BUTTON_WATER,INPUT_PULLUP);

  pinMode(LED_WATER,OUTPUT);
  pinMode(LED_ON,OUTPUT);
  pinMode(LED_LOADING,OUTPUT);
/*
  // Inicializar el LCD
  lcd.init();
  
  //Encender la luz de fondo.
  lcd.backlight();
  
  // Escribimos el Mensaje en el LCD.
  lcd.print("Hola Mundo");

*/
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
  /*
  delay(500);
  digitalWrite(LED_LOADING,HIGH);
  digitalWrite(LED_WATER,HIGH);
  digitalWrite(LED_ON,HIGH);
  delay(500);
  digitalWrite(LED_LOADING,LOW);
  digitalWrite(LED_WATER,LOW);
  digitalWrite(LED_ON,LOW);
  */
/*
 // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.setCursor(0, 1);
   // Escribimos el número de segundos trascurridos
  lcd.print(millis()/1000);
  lcd.print(" Segundos");
*/
}
