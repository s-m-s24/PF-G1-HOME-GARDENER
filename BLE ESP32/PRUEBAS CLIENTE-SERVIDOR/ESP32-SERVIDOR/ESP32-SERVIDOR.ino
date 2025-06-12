#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs compartidos con el cliente
#define SERVICE_UUID "8e91d054-a7dd-4318-859e-5a68c276faf3"
#define CHARACTERISTIC_UUID_TX "72dba8d8-ac05-4c11-94d4-05e5f8660259"  // cliente escribe acá
#define CHARACTERISTIC_UUID_RX "5fc3b348-023f-4ead-9cd5-624927d8fb65"  // cliente lee acá

BLECharacteristic *caracteristicaTX;  // cliente escribe
BLECharacteristic *caracteristicaRX;  // cliente lee

class CallbackEscritura : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String valor = String(pCharacteristic->getValue().c_str());
    if (valor.length() > 0) {
      String mensaje = String(valor.c_str());
      Serial.print("📩 Mensaje recibido: ");
      Serial.println(mensaje);

      // respuesta de confirmacion
      String respuesta = "Recibido: " + mensaje;
      caracteristicaRX->setValue(respuesta.c_str());
      Serial.print("📤 Enviando confirmación: ");
      Serial.println(respuesta);
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 Iniciando servidor BLE...");

  // inicializar BLE
  BLEDevice::init("ESP32 Servidor");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // característica para recibir mensajes del cliente
  caracteristicaTX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_WRITE);
  caracteristicaTX->setCallbacks(new CallbackEscritura());

  // característica para enviar confirmaciones al cliente
  caracteristicaRX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_READ);
  caracteristicaRX->addDescriptor(new BLE2902());

  // iniciar servicio
  pService->start();

  // anunciar el servicio
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("📡 Servidor BLE listo y esperando conexión...");
}

void loop() {
}
