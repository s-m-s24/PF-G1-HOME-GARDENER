#include <DHT.h>
#include <WiFi.h>
#include "time.h"
#include <esp_now.h>

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 10        //En segundos

unsigned long now = 0;
unsigned long lastAction = 0;
#define WAIT_TIME 1200000

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-IoT";
const char* password = "IoT$2026";

struct Messages {
  bool received;
  float tempRead;
  float humRead;
  String lastRead;
  bool watered;
  String lastWater;
  int IDPlant;
};

struct Reads {
  float tempRead;
  float humRead;
  String lastRead;
  bool watered;
};

Messages sendInfo = { false, 0, 0, "0", false, "0", 0 };
Reads sendReads = { 0, 0, "0", false };

String order = "";

int tempProblems = 0;
int humProblems = 0;
#define MAX_PROBLEMS 3
char humidDetected[70];
char tempDetected[70];
String instruction = "";
#define READ "read"
#define WATER "water"
#define READ_WATER "read_&_water"
#define RESEND "resend"

#define DHTPIN 4  // Pin donde está conectado el sensor
#define DHTTYPE DHT22

#define LED 17
#define PIN_HUMEDAD 15

#define CANT_READS 10
#define MIN_TEMP -20
#define MAX_TEMP 40
#define MIN_HUM 500
#define MAX_HUM 3800


/////////// SLEEP ///////////
#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 10        //En segundos
/////////////////////////////

String tempStatus = "";
String humStatus = "";

DHT dht(DHTPIN, DHTTYPE);
void ReadValues(void);
void States(void);
void Water(void);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;  // GMT-3 Argentina
const int daylightOffset_sec = 0;

#define SLEEP 0
#define ESPERA 1
#define LEER 2
#define REGAR 3
#define ENVIAR_INFO 4
int actualState = ESPERA;

  delay(1000);
  Serial.println("desperté");

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("El  ESP32 despierta cada: " + String(TIME_TO_SLEEP) + " Seconds");
  lastAction = millis();
}

void loop() {
  now = millis();
  States();
}

void States(void) {
  switch (actualState) {
    case ESPERA:
      if (instruction == READ || instruction == READ_WATER) {
        ReadValues();
        actualState = LEER;
      }
      if (instruction == WATER) {
        Water();
        actualState = REGAR;
      }
      if ((now - lastAction) >= WAIT_TIME) {
        //Go to sleep now
        esp_deep_sleep_start();
      }
      break;

    case LEER:
      if (humProblems >= MAX_PROBLEMS) {
        Serial.println(humidDetected);
        humProblems = 0;
        lastAction = millis();
        actualState = ESPERA;
      }
      if (tempProblems >= MAX_PROBLEMS) {
        Serial.println(tempDetected);
        tempProblems = 0;
        lastAction = millis();
        actualState = ESPERA;
      }
      if ((humProblems >= MAX_PROBLEMS) && (tempProblems >= MAX_PROBLEMS)) {
        //ENVIAR DATOS
        actualState = ENVIAR_INFO;
      }
      break;

    case REGAR:
      if (instruction == READ_WATER) {
        ReadValues();
        actualState = LEER;
      }
      if (instruction == WATER) {
        //ENVIAR DATOS
        actualState = ENVIAR_INFO;
      }
      break;

    case ENVIAR_INFO:
      //ENVIAR INFO Y
      lastAction = millis();
      actualState = ESPERA;
      break;
  }
}

void Water(void) {
  //PRENDER BOMBA POR 3 SEG
}

void ReadValues(void) {
  Serial.print("reading");
  unsigned long timestamp = getTime();
  int humid = 0;
  float temperature = 0;
  float promHumid = 0;
  float promTemp = 0;
  humid = analogRead(PIN_HUMEDAD);
  Serial.print(humid);
  Serial.print(" | ");
  Serial.println(temperature);
  temperature = dht.readTemperature();

  for (int i = 0; i < CANT_READS; i++) {
    Serial.print(".");
    delay(100);
    humid = analogRead(PIN_HUMEDAD);
    Serial.print(humid);
    Serial.print(" | ");
    Serial.println(temperature);
    temperature = dht.readTemperature();
    if ((temperature <= MIN_TEMP) || (temperature >= MAX_TEMP)) {
      //Serial.println("problem");
      tempProblems = tempProblems + 1;
      if (tempProblems >= MAX_PROBLEMS) {
        sprintf(tempDetected, "La temperatura fue: %.2f", temperature);
        //Serial.println(tempDetected);
        return;
      }
      i = i - 1;
    } else {
      promTemp = promTemp + temperature;
    }
    if ((humid <= MIN_HUM) || (humid >= MAX_HUM)) {
      //Serial.println("problem");
      humProblems = humProblems + 1;
      if (humProblems >= MAX_PROBLEMS) {
        sprintf(humidDetected, "La humedad fue: %d", humid);
        // Serial.println(humidDetected);
        return;
      }
      i = i - 1;
    } else {
      promHumid = promHumid + humid;
    }
  }
  Serial.println("");
  sendReads.tempRead = promTemp / CANT_READS;
  sendReads.humRead = promHumid / CANT_READS;

  sendInfo.tempRead = sendReads.tempRead;
  sendInfo.humRead = sendReads.humRead;
  sendInfo.lastRead = timestamp;

  //sendInfo.timestamp = timestamp;
  //Serial.println(sendInfo.timestamp);

  return;
}

unsigned long getTime() {
  time_t now;
  time(&now);
  return now;
}