#include <Arduino.h>
#include "esp_system.h" // 包含 esp_random()

// =========================================================
// 輔助函式：印出 Hex
// =========================================================
void printHex(uint8_t *data, int len) {
  for (int i = 0; i < len; i++) {
    Serial.printf("%02X ", data[i]);
  }
  Serial.println();
}

// =========================================================
// 應用 1: 生成真亂數金鑰 (適合給 AES 使用)
// =========================================================
void generateSecureKey() {
  uint8_t key[16];
  
  Serial.println("正在從 RF 熱雜訊中採集熵 (Entropy)...");
  
  // 利用硬體 RNG 填滿陣列
  // esp_random() 回傳 uint32_t (4 bytes)，所以要呼叫 4 次填滿 16 bytes
  // 或者使用 esp_fill_random(buffer, len) 更方便
  esp_fill_random(key, 16); 

  Serial.print("生成 128-bit 安全金鑰: ");
  printHex(key, 16);
}

// =========================================================
// Setup
// =========================================================
void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("\n\n=== ESP32 硬體真亂數產生器 (TRNG) ===");
  Serial.println("注意：ESP32 的亂數來自 Wi-Fi/藍牙 的底層射頻雜訊。");
  Serial.println("即使重開機，這些數字也絕對不會重複！");
  Serial.println("---------------------------------------------");
}

// =========================================================
// Loop
// =========================================================
void loop() {
  
  // 1. 讀取一個 32-bit 的原始硬體亂數
  uint32_t rawRand = esp_random();

  Serial.printf("[原始數據] %u (0x%08X)\n", rawRand, rawRand);

  // 2. 模擬應用：擲骰子 (0 ~ 99)
  // 使用取餘數運算
  int dice = rawRand % 100;
  Serial.printf("[擲骰子]   %d\n", dice);

  // 3. 模擬應用：生成加密金鑰
  generateSecureKey();

  Serial.println("---------------------------------------------");
  delay(2000); // 每兩秒產生一次
}
