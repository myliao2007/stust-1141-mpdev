#include <Arduino.h>
#include "mbedtls/sha256.h"

// =========================================================
// 全域變數
// =========================================================
String inputBuffer = "";
bool stringComplete = false;

// 存放 SHA-256 結果 (256 bits = 32 bytes)
uint8_t hashResult[32]; 

// =========================================================
// 核心函式：計算並印出 SHA-256
// =========================================================
void computeAndPrintSHA256(String input) {
  
  // 1. 準備 mbedtls context
  mbedtls_sha256_context ctx;
  mbedtls_sha256_init(&ctx);
  
  // 2. 開始運算
  // 第二個參數是用來選擇 SHA-224 或 SHA-256
  // 0 = SHA-256
  // 1 = SHA-224
  mbedtls_sha256_starts(&ctx, 0); 
  
  // 3. 餵入資料 (Update)
  // c_str() 會回傳字串的指標，length() 是長度
  // 這裡利用硬體加速器處理輸入資料
  unsigned long start_time = micros();
  
  mbedtls_sha256_update(&ctx, (const uint8_t*)input.c_str(), input.length());
  
  // 4. 結束並取得結果 (Finish)
  mbedtls_sha256_finish(&ctx, hashResult);
  
  unsigned long end_time = micros();

  // 5. 釋放資源
  mbedtls_sha256_free(&ctx);

  // 6. 顯示結果
  Serial.println("\n------------------------------------------------");
  Serial.print("輸入字串: ");
  Serial.println(input);
  Serial.printf("資料長度: %d bytes\n", input.length());
  Serial.printf("運算耗時: %lu us (硬體加速)\n", end_time - start_time);
  
  Serial.print("SHA-256 : ");
  // 迴圈印出 32 個 bytes 的十六進位值
  for (int i = 0; i < 32; i++) {
    Serial.printf("%02x", hashResult[i]);
  }
  Serial.println("\n------------------------------------------------");
  Serial.println("請輸入下一個字串...");
}

// =========================================================
// Setup
// =========================================================
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n\n=== ESP32 SHA-256 硬體雜湊計算機 ===");
  Serial.println("系統就緒。請輸入任意文字 (例如: hello)...");
}

// =========================================================
// Loop
// =========================================================
void loop() {
  // 處理 UART 輸入
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    // 遇到換行符號 (\n) 表示輸入結束
    if (inChar == '\n') {
      stringComplete = true;
    } 
    // 忽略 CR (\r)，避免 Windows/Linux 換行符號差異導致 Hash 不同
    else if (inChar != '\r') {
      inputBuffer += inChar;
    }
  }

  // 執行雜湊運算
  if (stringComplete) {
    if (inputBuffer.length() > 0) {
      computeAndPrintSHA256(inputBuffer);
    }
    
    // 清空 Buffer
    inputBuffer = "";
    stringComplete = false;
  }
}
