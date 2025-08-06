#include <DHT.h>
#include <WiFi.h>
#include "time.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-IoT";
const char* password = "IoT$2025";

struct PlantReads {
  int tempRead;
  int humRead;
  int lastRead;
  int lastWater;
};

struct IdealValues {
  int tempIdeal;
  int humIdeal;
};

#define DHTPIN 4  // Pin donde está conectado el sensor
#define DHTTYPE DHT22

#define LED 17
#define PIN_HUMEDAD 36
void ReadTemperature(void);
String tempStatus = "";
String humStatus = "";

DHT dht(DHTPIN, DHTTYPE);

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

  // Esperar hasta que se sincronice el tiempo
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Esperando sincronización NTP...");
    delay(500);
  }
  Serial.println("Hora sincronizada!");
}
void loop() {
  delay(1000);
  
  int lecturaHumedad = analogRead(PIN_HUMEDAD);
  float temperatura = dht.readTemperature();
  Serial.print(temperatura);
}
