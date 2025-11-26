#include <WiFi.h>
#include <HTTPClient.h>

// ================= Lab Network Settings =================
// Note: ESP32-S3 hardware only supports 2.4GHz WiFi.
const char* ssid = "INSLAB-2G";             
const char* password = "stust-eecs-2025"; 
const char* group_id = "Group-99";
// Target Raspberry Pi URL
const char* serverUrl = "http://192.168.50.184";
// ========================================================

// Request counter
int count = 0;

void init_wifi() {
  // 1. Set WiFi mode to Station
  WiFi.mode(WIFI_STA);
  
  // 2. Start connection
  Serial.printf("Connecting to WiFi: %s ...\n", ssid);
  WiFi.begin(ssid, password);

  // 3. Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // 4. Connection established
  Serial.println("\n\n----------------------------------");
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println("----------------------------------");
}

void get_pi_web(){
  if(WiFi.status() == WL_CONNECTED) {
    count++;
    HTTPClient http;

    Serial.printf("[#%d] [%s] Sending request to %s ...\n", count, group_id, serverUrl);

    // 1. Begin connection to the URL
    http.begin(serverUrl);
    http.setTimeout(3000); // Set timeout to 3 seconds

    // 2. Send GET request
    int httpCode = http.GET();

    // 3. Handle response
    if(httpCode > 0) {
      // Server responded
      if(httpCode == HTTP_CODE_OK) {
        // Get the response payload
        String payload = http.getString();
        
        Serial.print("[%s] --- Server Response (Start) ---", group_id);
        Serial.println(payload);
        Serial.print("[%s] --- Server Response (End) ---\n", group_id);
      } else {
        Serial.printf("Server responded with error code: %d\n\n", httpCode);
      }
    } else {
      // Connection failed
      Serial.printf("Connection failed! Error: %s\n\n", http.errorToString(httpCode).c_str());
    }

    // 4. Close connection
    http.end();

  } else {
    Serial.println("WiFi Disconnected. Attempting to reconnect...");
  }  
}

void setup() {
  // Initialize Serial. 
  // NOTE: "USB CDC On Boot" must be Enabled for N16R8 to print to Serial Monitor.
  Serial.begin(115200);
  delay(3000); // Wait for the USB Serial to initialize

  Serial.println("\n\n=== STUST Lab: Periodic HTTP Client Test ===");

  init_wifi();
}

void loop() {
  // Check WiFi status before sending request
  get_pi_web();

  // Wait for 5 seconds before the next request
  delay(5000);
}
