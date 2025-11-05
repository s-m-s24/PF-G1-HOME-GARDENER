#include <DHT.h>
#include <WiFi.h>
#include "time.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-IoT";
const char* password = "IoT$2025";

struct Messages {
  bool received;
  float tempRead;
  float humRead;
  String lastRead;
  bool watered;
  String lastWater;
  int IDPlant;
};

Messages sendInfo = { false, 0, 0, "0", false, "0", 0 };

String order = "";

int tempProblems = 0;
int humProblems = 0;
#define MAX_PROBLEMS 3
char humidDetected[70];
char tempDetected[70];
String instruction = "";
String orderRead = "read";

#define DHTPIN 4  // Pin donde está conectado el sensor
#define DHTTYPE DHT22

#define LED 17
#define PIN_HUMEDAD 15

#define CANT_READS 10
#define MIN_TEMP -20
#define MAX_TEMP 40
#define MIN_HUM 500
#define MAX_HUM 3800

String tempStatus = "";
String humStatus = "";

DHT dht(DHTPIN, DHTTYPE);
void ReadValues(void);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;  // GMT-3 Argentina
const int daylightOffset_sec = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(PIN_HUMEDAD, INPUT);
  pinMode(LED, OUTPUT);
  
  WiFi.begin(ssid, password);

  Serial.println("Connecting Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Esperando sincronización NTP...");
    delay(500);
  }
  Serial.println("Hora sincronizada!");  // Esperar hasta que se sincronice el tiempo
  
}
void loop() {
  if (Serial.available() > 0) {
    instruction = Serial.readStringUntil('\n');
    instruction.trim();
    if (instruction.equals(orderRead)) {
      Serial.println("to read");
      ReadValues();
    }
  }
  if (humProblems >= MAX_PROBLEMS) {
    Serial.println(humidDetected);
    humProblems = 0;
  }
  if (tempProblems >= MAX_PROBLEMS) {
    Serial.println(tempDetected);
    tempProblems = 0;
  }
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
  sendInfo.tempRead = promTemp / CANT_READS;
  sendInfo.humRead = promHumid / CANT_READS;
  sendInfo.lastRead = timestamp;
  Serial.println(sendInfo.humRead);
  Serial.println(sendInfo.tempRead);
  //Serial.println(sendReads.timestamp);
  return;
}


unsigned long getTime() {
  time_t now;
  time(&now);
  return now;
}