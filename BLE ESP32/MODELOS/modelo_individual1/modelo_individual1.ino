// CODIGO PARA EL ESP NUMERO 1
#include <esp_now.h>
#include <WiFi.h>

#define BOT_ESP1 27

#define ESTADO_ESPERA 1
#define ESTADO_CONFIRMAR_BOT1 2

int estadoActual = ESTADO_ESPERA;

bool flagBoton = false;

uint8_t esp3Adress[] = { 0xD4, 0x8A, 0xFC, 0xCF, 0x1F, 0x94 };  // central module adress --> d4:8a:fc:cf:1f:94

// Structure to receive data -- must match the sender structure
typedef struct central_actions_message {
  bool water_plants;
  bool send_data;  // send sensors data
  bool sleep;      // a dormir
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
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
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

void setup() {
  Serial.begin(115200);
  pinMode(BOT_ESP1, INPUT_PULLUP);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
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
}

void loop() {

  sendData.tempRead = 56;
  sendData.humRead = 15.4;
  sendData.timestamp = "hola";

  int lec_botesp1 = digitalRead(BOT_ESP1);
  switch (estadoActual) {
    case ESTADO_ESPERA:
      Serial.println("Estado Espera");

      if (myData.send_data == false) {
        Serial.println("Datos requeridos. Enviando a ESP3 (CENTRAL)...");
        esp_err_t result = esp_now_send(esp3Adress, (uint8_t *)&sendData, sizeof(sendData));
        if (result == ESP_OK) {
          Serial.println("Sent to ESP1 with success");
        } else {
          Serial.println("Error sending to ESP1");
        }
        myData.send_data = true;
      }
      break;

    case ESTADO_CONFIRMAR_BOT1:
      if (lec_botesp1 == HIGH) {
        flagBoton = true;
        estadoActual = ESTADO_ESPERA;
      }
      break;

    
  }
  delay (2000);
}
