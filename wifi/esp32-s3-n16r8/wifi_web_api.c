#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h> // [Added] Handle HTTPS connection
#include <ArduinoJson.h>      // [Added] Must install this library (Version 7)

// ================= Lab Network Settings =================
const char* ssid = "INSLAB-2G";             
const char* password = "stust-eecs-2025"; 
const char* group_id = "Group-99";

// [Modified] CWA API Settings
// Please enter your CWA API Authorization Key
String apiKey = "CWA-6090C5B6-E284-479D-931D-D6E2FF7ECB27"; 

// Set target city
// NOTE: This must match the API response (Chinese), do not change to English.
const char* targetCity = "臺南市";

// API URL: General Weather Forecast - Next 36 Hours
String serverUrl = "https://opendata.cwa.gov.tw/api/v1/rest/datastore/F-C0032-001?format=JSON&Authorization=";
// ========================================================

int count = 0;

void init_wifi() {
  WiFi.mode(WIFI_STA);
  Serial.printf("Connecting to WiFi: %s ...\n", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n\n----------------------------------");
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------");
}

// [Modified] Renamed function to get_weather for clarity
void get_weather(){
  if(WiFi.status() == WL_CONNECTED) {
    count++;
    
    // [Key Point 1] Establish secure connection (HTTPS)
    WiFiClientSecure client;
    client.setInsecure(); // Skip certificate validation to avoid connection failures due to expiration

    HTTPClient http;
    
    // Combine full URL
    String fullUrl = serverUrl + apiKey;

    Serial.printf("[#%d] [%s] Requesting Weather Data...\n", count, group_id);

    // [Key Point 2] Use begin with secure client
    if (http.begin(client, fullUrl)) {
      
      int httpCode = http.GET();

      if(httpCode > 0) {
        if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println("Data received. Parsing JSON...");

          // [Key Point 3] Parse JSON
          // CWA data is large, but N16R8 has enough memory, so declaring dynamic document directly is fine
          JsonDocument doc; 
          
          DeserializationError error = deserializeJson(doc, payload);

          if (error) {
            Serial.print("JSON Parsing Failed: ");
            Serial.println(error.c_str());
          } else {
            // [Key Point 4] Find data for Tainan City
            JsonArray locations = doc["records"]["location"];
            bool found = false;

            for (JsonObject city : locations) {
              // Compare locationName
              // Note: city["locationName"] returns Chinese characters (e.g., "臺南市")
              if (String((const char*)city["locationName"]) == targetCity) {
                found = true;
                
                // Get weatherElement Array
                JsonArray weatherElement = city["weatherElement"];
                
                // Parse values (Index: 0=Wx Weather Phenomenon, 1=PoP Rain Probability, 2=MinT Min Temp, 4=MaxT Max Temp)
                // Note: Get the 0th time slot (nearest forecast)
                String wx = weatherElement[0]["time"][0]["parameter"]["parameterName"];
                String pop = weatherElement[1]["time"][0]["parameter"]["parameterName"];
                String minT = weatherElement[2]["time"][0]["parameter"]["parameterName"];
                String maxT = weatherElement[4]["time"][0]["parameter"]["parameterName"];

                // Print formatted results
                Serial.printf("\n[%s] === %s Weather Forecast ===\n", group_id, targetCity);
                Serial.printf("Condition: %s\n", wx.c_str());
                Serial.printf("Rain Prob: %s %%\n", pop.c_str());
                Serial.printf("Temp Range: %s ~ %s C\n", minT.c_str(), maxT.c_str());
                Serial.println("==============================\n");
                break; 
              }
            }
            
            if(!found) Serial.println("City not found in response.");
          }

        } else {
          Serial.printf("Server Error: %d\n", httpCode);
        }
      } else {
        Serial.printf("Connection Failed: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.println("Unable to connect to server");
    }

  } else {
    Serial.println("WiFi Disconnected. Reconnecting...");
  }  
}

void setup() {
  Serial.begin(115200);
  delay(3000); 

  Serial.println("\n\n=== STUST Lab: CWA Weather API Test ===");

  init_wifi();
  
  // Fetch once on startup
  get_weather();
}

void loop() {
  // Weather data doesn't need frequent updates. 
  // Suggest 10 minutes (600000ms) or longer for real use.
  // Set to 30 seconds for demonstration here.
  delay(30000); 
  get_weather();
}
