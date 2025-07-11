
#include <WiFi.h>
#include "time.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-IoT";
const char* password = "IoT$2025";

// Connect to wifi
void  initWiFi(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
}

void loop() {    
 unsigned long timestamp = getTime();
 Serial.println(timestamp);
}

unsigned long getTime() {
 time_t now;
 struct tm timeinfo;
 time(&now);
 return now;
}
