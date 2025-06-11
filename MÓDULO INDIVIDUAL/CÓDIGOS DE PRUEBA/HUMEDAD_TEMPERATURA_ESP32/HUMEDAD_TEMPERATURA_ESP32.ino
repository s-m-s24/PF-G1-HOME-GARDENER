#include <DHT.h>

#define DHTPIN 4     // Pin donde está conectado el sensor
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(15, INPUT);
}
void loop() {
  delay(2000);
  int lectura = analogRead(15);
  float t = dht.readTemperature(); 
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" *C || Humedad: ");
  Serial.println(lectura); //lectura analógica
	//delay(100);
}

