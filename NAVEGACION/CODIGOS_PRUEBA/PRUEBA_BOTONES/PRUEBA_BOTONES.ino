#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_OK 4
#define BUTTON_WATER 5
#define PUSHED HIGH
#define N_PUSHED LOW
bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

void setup() {
  Serial.begin(9600);
}

void loop() {
  ReadButtonUp = digitalRead(BUTTON_UP);
  if (ReadButtonUp == PUSHED) {
    Serial.println("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }

  ReadButtonDown = digitalRead(BUTTON_DOWN);
  if (ReadButtonDown == PUSHED) {
    Serial.println("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }

  ReadButtonOk = digitalRead(BUTTON_OK);
  if (ReadButtonOk == PUSHED) {
    Serial.println("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }

  ReadButtonWater = digitalRead(BUTTON_WATER);
  if (ReadButtonWater == PUSHED) {
    Serial.println("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }
}
