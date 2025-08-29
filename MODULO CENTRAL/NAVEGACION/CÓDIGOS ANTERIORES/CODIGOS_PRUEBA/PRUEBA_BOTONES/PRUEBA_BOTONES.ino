#define BUTTON_UP 25
#define BUTTON_DOWN 26
#define BUTTON_OK 8
#define BUTTON_WATER 33

#define PUSHED LOW
#define N_PUSHED HIGH

  bool ReadButtonUp = N_PUSHED;
bool ReadButtonDown = N_PUSHED;
bool ReadButtonOk = N_PUSHED;
bool ReadButtonWater = N_PUSHED;

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);
  pinMode(BUTTON_WATER, INPUT_PULLUP);
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
  Serial.print(ReadButtonOk);
  Serial.print(" | OK: ");
  if (ReadButtonOk == PUSHED) {
    Serial.print("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }

  ReadButtonWater = digitalRead(BUTTON_WATER);
  Serial.print(" | WATER: ");
  if (ReadButtonWater == PUSHED) {
    Serial.println("PUSHED");
  } else {
    Serial.println("NOT PUSHED");
  }
}
