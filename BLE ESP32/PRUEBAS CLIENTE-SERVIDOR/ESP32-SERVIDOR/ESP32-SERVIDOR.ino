#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

//UUIDs compartidos
#define SERVICE_UUID "8e91d054-a7dd-4318-859e-5a68c276faf3"
#define CHARACTERISTIC_UUID_TX "72dba8d8-ac05-4c11-94d4-05e5f8660259"  // Cliente escribe
#define CHARACTERISTIC_UUID_RX "5fc3b348-023f-4ead-9cd5-624927d8fb65"  // Cliente recibe notificaciones

BLECharacteristic* caracteristicaTX;
BLECharacteristic* caracteristicaRX;

#define PIN_LED 2

class CallbackEscritura : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    String valor = pCharacteristic->getValue().c_str();
    if (valor.length() > 0) {
      String mensaje = String(valor.c_str());
      String mensaje_recortado;
      String mensaje_restante;
      bool flagCaracter = false;
      for (int i = 0; i < mensaje.length(); i++){
        char caracter = mensaje.charAt(i);
        if (flagCaracter == true) {
          mensaje_restante += caracter;
        }
        else if (caracter != '!') {
          mensaje_recortado += caracter;
        } 
        
        if (caracter == '!') {
          flagCaracter = true;
        }
      }
      Serial.print("📩 Mensaje recortado recibido: ");
      Serial.println(mensaje_recortado);

      if(mensaje_recortado == "SW1") {
        digitalWrite(PIN_LED, HIGH);
      } else {
        digitalWrite(PIN_LED, LOW);
      }
      // Enviar confirmación por notificación
      String respuesta = "Recibido: " + mensaje;

      caracteristicaRX->setValue(respuesta.c_str());
      caracteristicaRX->notify();

      Serial.print("📤 Enviando confirmación (notify): ");
      Serial.println(respuesta);
    }
  }
};

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED, OUTPUT);

  ////////// CONEXION SERVIDOR - CLIENTE //////////
  Serial.println("🚀 Iniciando servidor BLE...");

  BLEDevice::init("ESP32 Servidor");
  BLEServer* pServer = BLEDevice::createServer();
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // característica donde el cliente escribe
  caracteristicaTX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_WRITE);
  caracteristicaTX->setCallbacks(new CallbackEscritura());

  // característica donde el servidor envía notificaciones
  caracteristicaRX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
  caracteristicaRX->addDescriptor(new BLE2902());

  pService->start();

  // publicar servicio
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("📡 Servidor BLE listo y esperando conexión...");
  //////////////////////////////////////////////////////
}

void loop() {
  delay(1000);  // mantener loop vivo
}
