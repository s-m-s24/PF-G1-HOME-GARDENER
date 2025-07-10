#include <ESP32Time.h>
#include <WiFi.h>

//Otorgamos acceso a Time.h a wifi
const char* SSID = "ORT-IoT";
const char* PASSWORD = "NuevaIOT$25";
const char* NTP_SERVER = "pool.ntp.org";

unsigned long TiempoUltimoCambio = 0; //Variable que almacenará millis. Explicada mejor abajo
const long INTERVALO = 1000; //Cuanto tiempo dura el delay sin bloqueo
unsigned long TiempoAhora = millis(); //Guarda los milisegundos desde que se ejecutó el programa

ESP32Time rtc;

#define GMT_LOCAL 3


void setup() {
  // put your setup code here, to run once:
   WiFi.begin(SSID, PASSWORD);
   while (WiFi.status() != WL_CONNECTED) { //Mientras el wifi esté conectándose, que cada un intervalo imprima que intenta conectarse
    TiempoAhora = millis();
    if (TiempoAhora - TiempoUltimoCambio >= INTERVALO) {
      TiempoUltimoCambio = millis();
      Serial.println("Intentando conectar...");
    }
  }
  Serial.println("Conexión exitosa"); //Cuando sale del while (Se conecta), avisa del éxito de conexión
  establecerGMT();
}

void loop() {
  // put your main code here, to run repeatedly:
  String timeActual = rtc.getTime();
  Serial.println(timeActual);
}
