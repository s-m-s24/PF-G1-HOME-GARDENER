#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 20       //En segundos
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("desperté");

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  gpio_pullup_en(GPIO_NUM_27);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, LOW);
  Serial.println("El  ESP32 despierta cada: " + String(TIME_TO_SLEEP) + " Seconds");

  //Go to sleep now
  esp_deep_sleep_start();
}

void loop() {
  // put your main code here, to run repeatedly:
}
