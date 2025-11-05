#include <DHT.h>

#define DHTPIN 32     // Pin donde está conectado el sensor
#define DHTTYPE DHT22   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  ///pinMode(DHTPIN, OUTPUT);
}
void loop() {
  //igitalWrite(DHTPIN,LOW);
  delay(100);
  //digitalWrite(DHTPIN,HIGH);
  float t = dht.readTemperature(); 
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C");
  //Serial.println(analogRead(DHTPIN));
	delay(3000);
}

