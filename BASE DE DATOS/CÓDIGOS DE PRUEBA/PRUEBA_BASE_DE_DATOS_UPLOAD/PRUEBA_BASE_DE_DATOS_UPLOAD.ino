// Import WiFi and ESPSupabase Library
#include <ESPSupabase.h>
#include <WiFi.h>

// Add you Wi-Fi credentials
const char* ssid = "*Fibertel WiFi874 2.4GHz";
const char* password = "seg21NEWa1";

// Supabase credentials
const char* supabaseUrl = "https://jqjlakkgpogdskdxiymf.supabase.co";
const char* supabaseKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Impxamxha2tncG9nZHNrZHhpeW1mIiwicm9sZSI6InNlcnZpY2Vfcm9sZSIsImlhdCI6MTc0ODAwNjM5NCwiZXhwIjoyMDYzNTgyMzk0fQ.b0kKX838L8x0KjGZA1XCnwQL8MIQmQh8gSktdO3y_tM"; 

Supabase supabase;

void setup() {
 Serial.begin(115200);

 // Connect to Wi-Fi
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(1000);
   Serial.println("Connecting to Wi-Fi...");
 }
 Serial.println("Wi-Fi connected!");

 // Init Supabase
 supabase.begin(supabaseUrl, supabaseKey);

 // Add the table name here
 String tableName = "Estadisticas";
 // change the correct columns names you create in your table
 String payload = "{\"IdPlanta\": 5, \"Fecha\": 10.14, \"HumedadActual\": 14.3}";

 // sending data to supabase
 int response = supabase.insert(tableName, payload, false);
 if (response == 200) {
   Serial.println("Data inserted successfully!");
 } else {
   Serial.print("Failed to insert data. HTTP response: ");
   Serial.println(response);
   Serial.print ("Payload: ");
   Serial.println(payload);
 }
}

void loop() {
}
