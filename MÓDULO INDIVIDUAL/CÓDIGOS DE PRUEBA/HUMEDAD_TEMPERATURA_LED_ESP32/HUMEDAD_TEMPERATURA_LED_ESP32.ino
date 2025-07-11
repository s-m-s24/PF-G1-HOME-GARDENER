#include <DHT.h>

#define DHTPIN 4  // Pin donde está conectado el sensor
#define DHTTYPE DHT22

#define TEMP_IDEAL 20
#define TEMP_OK_RANGE 5
#define TEMP_EXCEED_RANGE 10
#define MIN_TEMP -20
#define MAX_TEMP 40

#define HUM_IDEAL 2050
#define HUM_OK_RANGE 300
#define HUM_EXCEED_RANGE 800
#define MIN_HUM 500
#define MAX_HUM 3800

#define LED 17
#define PIN_HUMEDAD 36
void ReadTemperature(void);
String tempStatus = "";
String humStatus = "";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(PIN_HUMEDAD, INPUT);
  pinMode(LED, OUTPUT);
}
void loop() {
  delay(1000);
  int lecturaHumedad = analogRead(PIN_HUMEDAD);
  float temperatura = dht.readTemperature();
  Serial.print(temperatura);
  tempStatus = filterReads(temperatura, MIN_TEMP, MAX_TEMP, TEMP_IDEAL, TEMP_OK_RANGE, TEMP_EXCEED_RANGE);
  //humStatus = filterReads(lecturaHumedad, MIN_HUM, MAX_HUM, HUM_IDEAL, HUM_OK_RANGE, HUM_EXCEED_RANGE);
}

String filterReads(float lecture, int MIN, int MAX, int IDEAL, int OK_RANGE, int EXCEED_RANGE) {
  /*
  requires: lecture, minimum, maximum, ideal value, permited range, exceeding range
  */
  String status = "";
  if ((lecture <= MIN) || (lecture >= MAX)) {
    status = "Ha ocurrido un problema";

  } else {
    if ((lecture <= IDEAL + OK_RANGE) && (lecture >= IDEAL - OK_RANGE)) {
      status ="IDEAL";
    } else if ((lecture <= IDEAL + EXCEED_RANGE) && (lecture >= IDEAL + OK_RANGE)) {
      status ="ALTO";
    } else if ((lecture >= IDEAL - EXCEED_RANGE) && (lecture <= IDEAL - OK_RANGE)) {
      status ="BAJ0";
    } else if (lecture <= IDEAL - EXCEED_RANGE) {
      status ="MUY BAJ0";
    } else if (lecture >= IDEAL + EXCEED_RANGE) {
      status ="MUY ALT0";
    }
  }
  Serial.print(" | ");
  Serial.println(status);
  return status;
}