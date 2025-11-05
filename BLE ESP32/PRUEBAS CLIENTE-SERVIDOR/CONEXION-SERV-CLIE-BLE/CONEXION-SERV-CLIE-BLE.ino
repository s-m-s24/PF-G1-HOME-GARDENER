#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEClient.h>
#include <BLE2902.h>

#define UUID_SERVICIO "8e91d054-a7dd-4318-859e-5a68c276faf3"
#define UUID_CARAC_TX "72dba8d8-ac05-4c11-94d4-05e5f8660259"
#define UUID_CARAC_RX "5fc3b348-023f-4ead-9cd5-624927d8fb65"

BLECharacteristic* caracteristicaTX;
BLECharacteristic* caracteristicaRX;
BLEServer* servidorBLE;
BLEClient* clienteBLE;
bool conectado = false;
bool comoCliente = false;

String ultimoMensajeRecibido = "";

// --- CALLBACK PARA EL SERVIDOR ---
class CallbackEscritura : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String valor = String((char*)pCharacteristic->getValue().c_str()); // guarda el msj recibido como "valor"
    Serial.print("📥 Recibido en modo servidor: ");
    Serial.println(valor);
    ultimoMensajeRecibido = valor;

    String confirmacion = "✔ Recibido: " + valor;
    caracteristicaTX->setValue(confirmacion.c_str());
    caracteristicaTX->notify();
  }
};

void iniciarServidor() { // prepara al ESP para comportatse como servidor BLE
  BLEDevice::init("ESP32 Dual");

  servidorBLE = BLEDevice::createServer();
  BLEService* servicio = servidorBLE->createService(UUID_SERVICIO);


  //////  CARACTERISTICAS ///////

  caracteristicaTX = servicio->createCharacteristic(
    UUID_CARAC_TX,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
  );
  caracteristicaTX->addDescriptor(new BLE2902());

  caracteristicaRX = servicio->createCharacteristic(
    UUID_CARAC_RX,
    BLECharacteristic::PROPERTY_WRITE
  );
  caracteristicaRX->setCallbacks(new CallbackEscritura());

  ///////////////////////////////


  servicio->start(); // inicio servidor 
  BLEDevice::getAdvertising()->start();
  Serial.println("🟢 Servidor BLE activo...");
}

bool conectarComoCliente() {
  BLEScan* scanner = BLEDevice::getScan();
  scanner->setActiveScan(true);
  BLEScanResults resultados = *scanner->start(3);

  for (int i = 0; i < resultados.getCount(); i++) {
    BLEAdvertisedDevice d = resultados.getDevice(i);
    if (d.haveServiceUUID() && d.isAdvertisingService(BLEUUID(UUID_SERVICIO))) {
      Serial.println("🔍 Encontrado servidor BLE");
      clienteBLE = BLEDevice::createClient();
      if (clienteBLE->connect(&d)) {
        BLERemoteService* servicio = clienteBLE->getService(UUID_SERVICIO);
        if (servicio == nullptr) return false;

        BLERemoteCharacteristic* caracRX = servicio->getCharacteristic(UUID_CARAC_RX);
        BLERemoteCharacteristic* caracTX = servicio->getCharacteristic(UUID_CARAC_TX);

        if (caracRX == nullptr || caracTX == nullptr) return false;

        String mensaje = "Hola desde el otro ESP";
        caracRX->writeValue((uint8_t*)mensaje.c_str(), mensaje.length(), false);

        String respuesta = String((char*)caracTX->readValue().c_str());
        Serial.print("📨 Confirmación: ");
        Serial.println(respuesta);
        return true;
      }
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  iniciarServidor();
}

void loop() {
  if (!comoCliente) {
    delay(10000); // Espera antes de cambiar a cliente
    Serial.println("🔁 Cambiando a modo cliente...");
    BLEDevice::deinit(); // cerrar servidor

    comoCliente = true;
    if (!conectarComoCliente()) {
      Serial.println("❌ Falló conexión como cliente");
    }
    BLEDevice::deinit(); // cerrar cliente
    iniciarServidor(); // reiniciar como servidor
    comoCliente = false;
  }

  delay(1000);
}
