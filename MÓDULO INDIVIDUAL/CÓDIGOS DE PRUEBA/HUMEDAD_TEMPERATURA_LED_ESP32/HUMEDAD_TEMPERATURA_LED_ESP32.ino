#include <DHT.h>

#include <WiFi.h>
#include "time.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-IoT";
const char* password = "IoT$2025";

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
}
void loop() {
  delay(1000);
  int lecturaHumedad = analogRead(PIN_HUMEDAD);
  float temperatura = dht.readTemperature();
  Serial.print(temperatura);
}
