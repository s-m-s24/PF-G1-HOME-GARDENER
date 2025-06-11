#include <DHT.h>

#define DHTPIN 4     // Pin donde está conectado el sensor
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}
void loop() {
  delay(2000);
  float t = dht.readTemperature(); 
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C");
	//delay(100);
}

