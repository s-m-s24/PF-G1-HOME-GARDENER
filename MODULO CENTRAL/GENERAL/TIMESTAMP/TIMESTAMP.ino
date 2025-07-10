/*#include <stdio.h>
#include <time.h>

int main ()
{
    time_t seconds;
    
    seconds = time(NULL);
    printf("Seconds since January 1, 1970 = %ld\n", seconds);
    
    return(0);
}*/

#include <WiFi.h>
#include "time.h"

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "MECA-IoT";
const char* password = "IoT$2025";

// REPLACE WITH YOUR TIMEZONE STRING: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
String myTimezone ="WET0WEST,M3.5.0/1,M10.5.0";

// Connect to wifi
void  initWiFi(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
}

// Function to set timezone
void setTimezone(String timezone){
  Serial.printf("  Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

// Connect to NTP server and adjust timezone
void initTime(String timezone){
  struct tm timeinfo;
  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo)){
    Serial.println(" Failed to obtain time");
    return;
  }
  Serial.println("Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

// Get the picture filename based on the current ime
String getPictureFilename(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "";
  }
  char timeString[20];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d_%H-%M-%S", &timeinfo);
  Serial.println(timeString);
  String filename = "/picture_" + String(timeString) +".jpg";
  return filename; 
}

// Initialize the micro SD card
void initMicroSDCard(){
  // Start Micro SD card
  Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }
}

// Take photo and save to microSD card
void takeSavePhoto(){
  // Take Picture with Camera
  camera_fb_t * fb = esp_camera_fb_get();
 
  //Uncomment the following lines if you're getting old pictures
  //esp_camera_fb_return(fb); // dispose the buffered image
  //fb = NULL; // reset to capture errors
  //fb = esp_camera_fb_get();
  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }

  // Path where new picture will be saved in SD Card
  String path = getPictureFilename();
  Serial.printf("Picture file name: %s\n", path.c_str());
  
  // Save picture to microSD card
  fs::FS &fs = SD_MMC; 
  File file = fs.open(path.c_str(),FILE_WRITE);
  if(!file){
    Serial.printf("Failed to open file in writing mode");
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved: %s\n", path.c_str());
  }
  file.close();
  esp_camera_fb_return(fb); 
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Initialize Wi-Fi
  initWiFi();
  // Initialize time with timezone
  initTime(myTimezone);
}

void loop() {    
  // Take and Save Photo
  takeSavePhoto();
  delay(10000);
}