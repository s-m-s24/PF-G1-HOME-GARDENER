#define BUTTON_UP 5
#define BUTTON_DOWN 4
#define BUTTON_OK 16
#define BUTTON_WATER 17

#define LED_WATER 18
#define LED_LOADING 19
#define LED_ON 21

#define PUSHED LOW
#define N_PUSHED HIGH

bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_UP,INPUT_PULLUP);
  pinMode(BUTTON_DOWN,INPUT_PULLUP);
  pinMode(BUTTON_OK,INPUT_PULLUP);
  pinMode(BUTTON_WATER,INPUT_PULLUP);

  pinMode(LED_WATER,OUTPUT);
  pinMode(LED_ON,OUTPUT);
  pinMode(LED_LOADING,OUTPUT);
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
  
  delay(500);
  digitalWrite(LED_LOADING,HIGH);
  digitalWrite(LED_WATER,HIGH);
  digitalWrite(LED_ON,HIGH);
  delay(500);
  digitalWrite(LED_LOADING,LOW);
  digitalWrite(LED_WATER,LOW);
  digitalWrite(LED_ON,LOW);
}
