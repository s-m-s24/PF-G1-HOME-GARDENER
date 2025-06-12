#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>

// UUIDs (asegúrate de que coincidan con los del servidor)
#define SERVICE_UUID "8e91d054-a7dd-4318-859e-5a68c276faf3"
#define CHARACTERISTIC_UUID_TX "72dba8d8-ac05-4c11-94d4-05e5f8660259"
#define CHARACTERISTIC_UUID_RX "5fc3b348-023f-4ead-9cd5-624927d8fb65"

BLEClient* clienteBLE;
BLERemoteCharacteristic* caracteristicaTX;
BLERemoteCharacteristic* caracteristicaRX;
bool conectado = false;

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32 Cliente");

  BLEScan* scanner = BLEDevice::getScan();
  scanner->setActiveScan(true);

  Serial.println("🔍 Escaneando dispositivos BLE...");
  BLEScanResults* resultados = scanner->start(5);  // 5 segundos

  for (int i = 0; i < resultados->getCount(); i++) {
    BLEAdvertisedDevice d = resultados->getDevice(i);

    if (d.haveServiceUUID() && d.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      Serial.print("✅ Dispositivo encontrado: ");
      Serial.println(d.toString().c_str());

      clienteBLE = BLEDevice::createClient();

      if (clienteBLE->connect(&d)) {
        Serial.println("🎉 Conectado al servidor BLE");

        BLERemoteService* servicio = clienteBLE->getService(SERVICE_UUID);
        if (servicio == nullptr) {
          Serial.println("❌ Servicio no encontrado");
          return;
        }

        caracteristicaTX = servicio->getCharacteristic(CHARACTERISTIC_UUID_TX);
        caracteristicaRX = servicio->getCharacteristic(CHARACTERISTIC_UUID_RX);

        if (caracteristicaTX == nullptr || caracteristicaRX == nullptr) {
          Serial.println("❌ Características no encontradas");
          return;
        }

        conectado = true;
        break;
      } else {
        Serial.println("Falló la conexión");
      }
    }
  }

  if (!conectado) {
    Serial.println("No se encontró un servidor BLE compatible");
  }
}

void loop() {
  if (conectado) {
    String mensaje = "Hola desde cliente";
    Serial.print("📤 Enviando mensaje: ");
    Serial.println(mensaje);

    caracteristicaTX->writeValue((uint8_t*)mensaje.c_str(), mensaje.length());

    delay(300);  // espera para que el servidor responda

    String respuesta = caracteristicaRX->readValue().c_str();
    Serial.print("📩 Confirmación del servidor: ");
    Serial.println(respuesta.c_str());
  }

  delay(10000);  
}
