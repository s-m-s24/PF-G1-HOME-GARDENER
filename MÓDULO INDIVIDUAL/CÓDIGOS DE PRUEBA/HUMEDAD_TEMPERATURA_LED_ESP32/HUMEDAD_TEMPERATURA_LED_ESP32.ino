#include <DHT.h>

#define DHTPIN 4     // Pin donde está conectado el sensor
#define DHTTYPE DHT22 

#define LED 17
#define PIN_HUMEDAD 36

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(PIN_HUMEDAD, INPUT);
  pinMode(LED,OUTPUT);
}
void loop() {
  delay(1000);
  int lecturaHumedad = analogRead(PIN_HUMEDAD);
  float temperatura = dht.readTemperature(); 
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" *C || Humedad: ");
  Serial.println(lecturaHumedad); //lectura analógica
	//delay(100);
  digitalWrite(LED,HIGH);
  delay(1000);
  digitalWrite(LED,LOW);
}

