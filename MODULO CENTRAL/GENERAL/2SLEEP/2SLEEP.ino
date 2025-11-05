#include <List.hpp>
#include <ESP32Time.h>
#include <WiFi.h>

int GMTActual = -3;

const char* ssid = "ORT-IoT";
const char* password = "NuevaIOT$25";
const char* NTP_SERVER = "pool.ntp.org";

ESP32Time rtc;
int tiempoAhora;
int tiempoUltimoCambio;
#define INTERVALO 1000

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 2         //En segundos

List<int> wakeupHours;
List<int> wakeupMins;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("desperté");

  
  wakeupHours.add(8);
  wakeupHours.add(15);
  wakeupHours.add(22);
  

  wakeupMins.add(0);
  wakeupMins.add(5);
  wakeupMins.add(10);
  wakeupMins.add(15);
  wakeupMins.add(20);
  wakeupMins.add(25);
  wakeupMins.add(30);
  wakeupMins.add(35);
  wakeupMins.add(40);
  wakeupMins.add(45);
  wakeupMins.add(50);
  wakeupMins.add(55);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  gpio_pullup_en(GPIO_NUM_27);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, LOW);
  Serial.println("El  ESP32 despierta cada: " + String(TIME_TO_SLEEP) + " Seconds");

  //Go to sleep now
  esp_deep_sleep_start();

  WiFi.begin(ssid, password);
  Serial.println("Conectando a wifi");
  while (WiFi.status() != WL_CONNECTED) {
    tiempoAhora = millis();
    if (tiempoAhora - tiempoUltimoCambio >= INTERVALO) {
      tiempoUltimoCambio = millis();
      Serial.println("Intentando conectar...");
    }
  }
  Serial.println("Conexión exitosa");
  establecerGMT();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentHour = rtc.getHour(true);
  unsigned long currentMin = rtc.getMinute();
  GoToSleep();
  Serial.println(currentMin);
}

void GoToSleep(){
  for (int hour = 0; hour <= wakeupHours.getSize(); hour++){
    if (currentHour<wakeupHours.get(hour)){

    }
  }
  return;
}

void establecerGMT() {
  const int HORAS_A_SEGUNDOS = 3600;
  const int DAYLIGHT_OFFSET = 0;
  long gmtOffset_sec = GMTActual * HORAS_A_SEGUNDOS;
  configTime(gmtOffset_sec, DAYLIGHT_OFFSET, NTP_SERVER);


  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
  } else {
    Serial.println("Hubo un error");
  }
  return;
}