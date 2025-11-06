// este ahora es el codigo de structs y timestamp pero lo pongo aca porque necesito hacer el merge jiji

#include <DHT.h>
#include <WiFi.h>
#include "time.h"
#include <esp_now.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-IoT";
const char* password = "IoT$2025";

struct Reads {
  float tempRead;
  float humRead;
  String lastRead;
  String lastWater;
  String timestamp;
};


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


/////////// SLEEP ///////////
#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 10        //En segundos
/////////////////////////////


String tempStatus = "";
String humStatus = "";

DHT dht(DHTPIN, DHTTYPE);
void ReadValues(void);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800;  // GMT-3 Argentina
const int daylightOffset_sec = 0;

Reads sendReads = { 0, 0, "0", "0", "0" };

/////////// BLE ///////////

uint8_t esp3Adress[] = { 0xC0, 0x49, 0xEF, 0x69, 0xCB, 0x48 };  // central module adress --> d4:8a:fc:cf:1f:94
// 

typedef struct central_actions_message {
  int water_plants;
  int send_data;  // send sensors data
  int sleep;      // a dormir
} central_actions_message;

// STRUCT QUE VA A DEVOLVER
typedef struct individual_data_message {
  float tempRead;
  float humRead;
  String lastRead;
  String lastWater;
  String timestamp;
} individual_data_message;

// create a struct called myData to save the recieved actions from the central module
central_actions_message myData;
// create a sendData struct
individual_data_message sendData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("WATER PLANTS: ");
  Serial.println(myData.water_plants);
  Serial.print("SEND SENSOR DATA: ");
  Serial.println(myData.send_data);
  Serial.print("SLEEP: ");
  Serial.println(myData.sleep);
  Serial.println();
}
///////////////////////////


void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(PIN_HUMEDAD, INPUT);
  pinMode(LED, OUTPUT);
  WiFi.mode(WIFI_STA);

  ////////////////////////////////////// BLE //////////////////////////////////////

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, esp3Adress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  ///////////////////////////////////////////////////////////////////////////////




  /*
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
  */
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

  if(myData.water_plants == 1){
    Serial.println("regar");
    myData.water_plants = 0;
  }
  if (myData.send_data == 1) { // si pidió la data
    Serial.println("Datos requeridos. Enviando a ESP3 (CENTRAL)...");
    esp_err_t result = esp_now_send(esp3Adress, (uint8_t*)&sendData, sizeof(sendData));
    if (result == ESP_OK) {
      Serial.println("Sent to ESP3 (CENTRAL) with success");
    } else {
      Serial.println("Error sending to ESP3 (CENTRAL)");
    }
    myData.send_data = 0;
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
  sendReads.tempRead = promTemp / CANT_READS;
  sendReads.humRead = promHumid / CANT_READS;

  sendData.tempRead = sendReads.tempRead;
  sendData.humRead = sendReads.humRead;
  sendData.timestamp = timestamp;

  


return;
}

//sendReads.timestamp = timestamp;
//Serial.println(sendReads.timestamp);





unsigned long getTime() {
  time_t now;
  time(&now);
  return now;
}