#define PIN_HUMEDAD 15

void setup() {
	Serial.begin(9600);
	pinMode(PIN_HUMEDAD, INPUT);
}

void loop() {
  int lecturaHumedad = analogRead(PIN_HUMEDAD);
	Serial.println(lecturaHumedad); //lectura analógica
	//Serial.println(digitalRead(10)); //lectura digital
	delay(100);
}