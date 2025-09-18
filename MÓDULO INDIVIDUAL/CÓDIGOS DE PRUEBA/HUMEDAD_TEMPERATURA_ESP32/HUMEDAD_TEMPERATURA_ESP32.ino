#include <DHT.h>

//#define DHTPIN 4     // Pin donde está conectado el sensor
//#define DHTTYPE DHT22
#define PINLED 2

#define PIN_HUMEDAD 36

//DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  //dht.begin();
  pinMode(PIN_HUMEDAD, INPUT);
  pinMode(PINLED, OUTPUT);
}
void loop() {
  delay(2000);
  int lecturaHumedad = analogRead(PIN_HUMEDAD);
  //float temperatura = dht.readTemperature();

  if (lecturaHumedad < 3000) {
    digitalWrite(PINLED, HIGH);

  } else {
    digitalWrite(PINLED, LOW);
  }

  //Serial.print("Temperatura: ");
  //Serial.print(temperatura);
  Serial.print("Humedad: ");
  Serial.println(lecturaHumedad);  //lectura analógica
}