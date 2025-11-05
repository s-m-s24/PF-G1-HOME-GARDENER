#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Sofi"; //"*Fibertel WiFi874 2.4GHz"
const char* password =  "IngEloM2025"; //"seg21NEWa1"
const char* apiBaseUrl =  "http://172.29.204.17:3000"; //"http:192.168.0.167:3000"

const char* userEmail = "samasofiacuenta12@gmail.com";
const char* userPassword = "HomeGarden25";

bool couldLogin = false;

String jwtToken = "";
void setup() {
  Serial.begin(115200);

  // Conectar al WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("WiFi conectado!");

  // 1️⃣ Hacer login
  if (loginAPI()) {
    Serial.println("Login exitoso, token obtenido:");
    Serial.println(jwtToken);

    // 2️⃣ Enviar datos de sensores
    enviarDatosSensor(25.5, 60.2);
  } else {
    Serial.println("Error en login, no se pudo obtener token.");
  }
}

void loop() {
  // aquí podrías enviar datos cada cierto tiempo si querés
  delay(60000);  // cada 1 minuto
}

// ========================
// FUNCIONES
// ========================

// Función para login y guardar el token
bool loginAPI() {
  HTTPClient http;
  String url = String(apiBaseUrl) + "/api/auth/login";
  http.begin(url);

  http.addHeader("Content-Type", "application/json");

  // JSON del login
  String payload = "{\"email\":\"" + String(userEmail) + "\",\"password\":\"" + String(userPassword) + "\"}";

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Respuesta login:");
    Serial.println(response);

    // Parsear JSON con ArduinoJson
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (!error && doc.containsKey("token")) {
      jwtToken = doc["token"].as<String>();
      http.end();
      return true;
    }
  } else {
    Serial.print("Error en login, código: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  return false;
}

// Función para enviar datos de sensor
void enviarDatosSensor(float temperatura, float humedad) {
  if (jwtToken == "") {
    Serial.println("No hay token disponible, abortando envío.");
    return;
  }

  HTTPClient http;
  String url = String(apiBaseUrl) + "/sensores";
  http.begin(url);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + jwtToken);

  // Crear JSON de datos
  String payload = "{\"temperatura\":" + String(temperatura, 2) + ",\"humedad\":" + String(humedad, 2) + "}";

  int httpResponseCode = http.POST(payload);
  Serial.print("Código de respuesta sensores: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Respuesta servidor:");
    Serial.println(response);
  } else {
    Serial.print("Error al enviar datos, código: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}