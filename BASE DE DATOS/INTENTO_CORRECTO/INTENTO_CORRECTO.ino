#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "MECA-IoT"; //"*Fibertel WiFi874 2.4GHz"
const char* password =  "IoT$2025"; //"seg21NEWa1"
const char* server =  "http://192.168.5.82:3000"; //"http:192.168.0.167:3000"

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");

  HTTPClient http;
  http.begin(String(server) + "/api/auth/login");
  http.addHeader("Content-Type", "application/json");

  // 🟢 Construimos el JSON correctamente
  StaticJsonDocument<200> doc;
  doc["email"] = "samasofiacuenta12@gmail.com";
  doc["password"] = "HomeGarden25";

  String requestBody;
  serializeJson(doc, requestBody);

  Serial.println("JSON que se enviará:");
  Serial.println(requestBody);

  int httpResponseCode = http.POST(requestBody);

  Serial.print("Código de respuesta HTTP: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Respuesta del servidor:");
    Serial.println(response);
  } else {
    Serial.print("Error en conexión HTTP: ");
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}

void loop() {}
