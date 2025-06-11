void setup() {
	Serial.begin(9600);
	pinMode(15, INPUT);
}

void loop() {
  int lectura = analogRead(15);
	Serial.println(lectura); //lectura analógica
	//Serial.println(digitalRead(10)); //lectura digital
	delay(100);
}