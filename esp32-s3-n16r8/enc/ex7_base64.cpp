#include <Arduino.h>
#include "mbedtls/base64.h"

// 全域變數：處理 Serial 輸入
String inputBuffer = "";
bool stringComplete = false;

// =========================================================
// 核心函式：解碼並印出 ASCII
// =========================================================
void decodeToASCII(String input) {
  
  // 1. 清理輸入 (Base64 非常討厭換行符號)
  input.trim();
  int inputLen = input.length();

  if (inputLen == 0) return;

  // 2. 準備記憶體
  // 解碼後長度約為原本的 3/4，多加 1 byte 是為了放字串結尾 '\0'
  int outputLen = (inputLen * 3 / 4) + 10; 
  unsigned char *decodedBuffer = (unsigned char *)malloc(outputLen);
  
  if (!decodedBuffer) {
    Serial.println("記憶體不足！");
    return;
  }

  // 3. 執行解碼
  size_t written_len = 0;
  int ret = mbedtls_base64_decode(decodedBuffer, outputLen, &written_len, 
                                  (const unsigned char *)input.c_str(), inputLen);

  Serial.println("\n-------------------------------------------");
  
  if (ret == 0) {
    // 【關鍵步驟】補上字串結束符號 (Null Terminator)
    // C 語言的字串必須以 0 結尾，否則會印出亂碼
    decodedBuffer[written_len] = '\0';

    Serial.print("原始編碼: ");
    Serial.println(input);
    
    Serial.print("解碼結果: ");
    // 直接轉型印出 ASCII 文字
    Serial.println((char *)decodedBuffer);
    
  } else {
    Serial.println("❌ 解碼失敗 (格式錯誤)");
  }
  
  Serial.println("-------------------------------------------");
  Serial.println("請輸入下一個 Base64 字串...");

  // 4. 釋放記憶體
  free(decodedBuffer);
}

// =========================================================
// Setup
// =========================================================
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n\n=== ESP32 Base64 -> ASCII 解碼器 ===");
  Serial.println("請輸入編碼字串 (例如: SGVsbG8gV29ybGQ= )");
}

// =========================================================
// Loop
// =========================================================
void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputBuffer += inChar;
    }
  }

  if (stringComplete) {
    if (inputBuffer.length() > 0) {
      decodeToASCII(inputBuffer);
    }
    inputBuffer = "";
    stringComplete = false;
  }
}
