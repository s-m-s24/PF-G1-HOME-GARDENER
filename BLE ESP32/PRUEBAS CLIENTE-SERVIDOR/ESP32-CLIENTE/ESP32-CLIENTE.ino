#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>

// UUIDs (deben coincidir con los del servidor)
#define SERVICE_UUID "8e91d054-a7dd-4318-859e-5a68c276faf3"
#define CHARACTERISTIC_UUID_TX "72dba8d8-ac05-4c11-94d4-05e5f8660259"
#define CHARACTERISTIC_UUID_RX "5fc3b348-023f-4ead-9cd5-624927d8fb65"

BLEClient* clienteBLE;
BLERemoteCharacteristic* caracteristicaTX;
BLERemoteCharacteristic* caracteristicaRX;
bool conectado = false;

#define SW1 27

void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                    uint8_t* pData, size_t length, bool isNotify) {
  String respuesta = String((char*)pData);
  Serial.print("📩 Confirmación del servidor (notify): ");
  Serial.println(respuesta);
}

void setup() {
  Serial.begin(115200);

  pinMode(SW1, INPUT_PULLUP);


  ///// CONEXION CLIENTE-SERVIDOR /////
  BLEDevice::init("ESP32 Cliente");

  BLEScan* scanner = BLEDevice::getScan();
  scanner->setActiveScan(true);
  Serial.println("🔍 Escaneando dispositivos BLE...");
  BLEScanResults resultados = *scanner->start(10);  // 10 segundos

  for (int i = 0; i < resultados.getCount(); i++) {
    BLEAdvertisedDevice d = resultados.getDevice(i);
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

        caracteristicaRX->registerForNotify(notifyCallback);

        conectado = true;
        break;
      } else {
        Serial.println("❌ Falló la conexión al servidor");
      }
    }
  }

  if (!conectado) {
    Serial.println("🚫 No se encontró un servidor BLE compatible");
  }
  ////////////////////////////////////
}


void loop() {
  int estadoSW1 = digitalRead(SW1) ;

  ///// FUNCIONAMIENTO CUANDO ESTÁ CONECTADO /////
  if (conectado) { 
    if (estadoSW1 == LOW) {

      String mensaje = "SW1!WOWWOWOW";
      Serial.print("📤 Enviando mensaje: ");
      Serial.println(mensaje);

      caracteristicaTX->writeValue((uint8_t*)mensaje.c_str(), mensaje.length());
    }


    // delay(10000);  // espera antes de enviar otro mensaje
  }
  //// si no se conecta, vuelve a intentar en 5 segs
  else {
    Serial.println("⏳ Cliente aún no conectado...");
    delay(5000);
  }
}
