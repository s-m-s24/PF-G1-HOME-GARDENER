
#include <esp_now.h>
#include <WiFi.h>

#define BOT_ESP2 33
#define BOT_ESP1 27

#define ESTADO_CONFIRMAR_BOT2 1
#define ESTADO_CONFIRMAR_BOT1 3
#define ESTADO_ESPERA 2

int estadoActual = ESTADO_ESPERA;
bool flagBot2 = false;
bool flagBot1 = false;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t esp2Adress[] = { 0xC0, 0x49, 0xEF, 0x69, 0xCB, 0x48 };
uint8_t esp1Adress[] = { 0xB8, 0xD6, 0x1A, 0xA7, 0x6F, 0xB8 };  // direccion mac esp1 b8:d6:1a:a7:6f:b8


// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a struct_message called myData
struct_message myData;
struct_message myDataEsp1;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(BOT_ESP2, INPUT_PULLUP);
  pinMode(BOT_ESP1, INPUT_PULLUP);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, esp2Adress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer 2");
    return;
  }

  memcpy(peerInfo.peer_addr, esp1Adress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer 1");
  }
  // Add peer
}

void loop() {
  // Set values to send
  int lec_botesp2 = digitalRead(BOT_ESP2);
  int lec_botesp1 = digitalRead(BOT_ESP1);

  strcpy(myData.a, "THIS IS A CHAR");
  myData.b = random(1, 20);
  myData.c = 1.2;
  myData.d = false;

  strcpy(myDataEsp1.a, "char esp1");
  myDataEsp1.b = random(1, 3);
  myDataEsp1.c = 0.8;
  myDataEsp1.d = true;

  switch (estadoActual) {

    case ESTADO_ESPERA:
      Serial.println("estado espera");
      if (lec_botesp2 == LOW) {
        estadoActual = ESTADO_CONFIRMAR_BOT2;
      }

      if (lec_botesp1 == LOW) {
        estadoActual = ESTADO_CONFIRMAR_BOT1;
      }

      if (flagBot2 == true) {
        // envía mensaje
        Serial.println("flag2 true");
        esp_err_t result = esp_now_send(esp2Adress, (uint8_t *)&myData, sizeof(myData));

        if (result == ESP_OK) {
          Serial.println("Sent with success");
        } else {
          Serial.println("Error sending the data");
        }
        flagBot2 = false;
      }

      if (flagBot1 == true) {
        Serial.println("Boton 1 presionado. Enviando a esp1Adress...");
        esp_err_t result = esp_now_send(esp1Adress, (uint8_t *)&myDataEsp1, sizeof(myDataEsp1));
        if (result == ESP_OK) {
          Serial.println("Sent to ESP1 with success");
        } else {
          Serial.println("Error sending to ESP1");
        }
                flagBot1 = false;

      }
      break;

    case ESTADO_CONFIRMAR_BOT2:
      Serial.println("estado confirmar boton");
      if (lec_botesp2 == HIGH) {
        flagBot2 = true;
        estadoActual = ESTADO_ESPERA;
      }
      break;

    case ESTADO_CONFIRMAR_BOT1:
      if (lec_botesp1 == HIGH) {
        flagBot1 = true;
        estadoActual = ESTADO_ESPERA;
      }
      break;
  }


  // Send message via ESP-NOW

  delay(2000);
}