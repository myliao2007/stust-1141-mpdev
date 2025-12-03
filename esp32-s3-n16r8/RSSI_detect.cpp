/*
 * 實驗：Wi-Fi RSSI 雷達
 * 請開啟手機熱點 (名稱改為 teacher_phone)，靠近板子看燈色變化
 */
#include <WiFi.h>

const char* TARGET_SSID = "Aaron"; // 請改成你的手機熱點名稱
const int RGB_PIN = 48; // S3 內建 LED

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  neopixelWrite(RGB_PIN, 0, 0, 0);
  Serial.println("Radar Started...");
}

void loop() {
  // 掃描網路
  int n = WiFi.scanNetworks();
  bool found = false;
  int targetRSSI = -100;

  for (int i = 0; i < n; ++i) {
    if (WiFi.SSID(i) == TARGET_SSID) {
      targetRSSI = WiFi.RSSI(i);
      found = true;
      break;
    }
  }

  if (found) {
    Serial.print("Target Found! RSSI: ");
    Serial.println(targetRSSI);
    
    if (targetRSSI > -50) {
      neopixelWrite(RGB_PIN, 50, 0, 0); 
    } else if (targetRSSI > -70) {
      neopixelWrite(RGB_PIN, 0, 50, 0); 
    } else {
      neopixelWrite(RGB_PIN, 0, 0, 50); 
    }
  } else {
    Serial.println("Target lost...");
    neopixelWrite(RGB_PIN, 10, 10, 10); // 白光閃爍表示搜尋中
    delay(100);
    neopixelWrite(RGB_PIN, 0, 0, 0);
  }

  WiFi.scanDelete(); // 清除記憶體
  delay(500); // 掃描頻率
}
