#include <WiFi.h>
#include "time.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-Iot";
const char* password = "IoT$2025";

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;  // GMT-3 Argentina
const int daylightOffset_sec = 0;

void setup() {
  Serial.begin(9600);
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
  unsigned long timestamp = getTime();
  Serial.println(timestamp);
  delay(1000);
}

unsigned long getTime() {
  time_t now;
  time(&now);
  return now;
}
