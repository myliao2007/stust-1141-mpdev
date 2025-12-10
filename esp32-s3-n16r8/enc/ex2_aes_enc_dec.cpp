#include <Arduino.h>
#include "mbedtls/aes.h"

// =========================================================
// 全域變數與緩衝區
// =========================================================
mbedtls_aes_context aes;
String inputBuffer = "";
bool stringComplete = false;

// 狀態機定義
enum ProgramState {
  WAIT_ENC_KEY,  // 0: 等待輸入加密 Key
  WAIT_MSG,      // 1: 等待輸入明文訊息
  WAIT_DEC_KEY   // 2: 等待輸入解密 Key
};
ProgramState currentState = WAIT_ENC_KEY;

// 存放 Key (16 Bytes)
uint8_t encryptionKey[16];
uint8_t decryptionKey[16];

// 存放密文 (Ciphertext) 的全域緩衝區
// 假設最大處理 256 bytes 的訊息
uint8_t storedCiphertext[256]; 
int storedLength = 0;          // 紀錄密文的實際長度

// =========================================================
// 輔助函式：將字串轉為 16 Bytes Key (不足補0)
// =========================================================
void parseKey(String input, uint8_t *keyArray) {
  memset(keyArray, 0, 16); // 先清空
  int len = input.length();
  
  // 複製資料 (最多 16 bytes)
  for (int i = 0; (i < len) && (i < 16); i++) {
    keyArray[i] = input[i];
  }
}

// =========================================================
// 階段 1 & 2: 設定加密 Key 並執行加密
// =========================================================
void performEncryption(String message) {
  // 1. 計算 Padding (補零至 16 的倍數)
  int msgLen = message.length();
  int remainder = msgLen % 16;
  int padding = 16 - remainder;
  storedLength = msgLen + padding; // 更新全域長度

  if (storedLength > 256) {
    Serial.println("錯誤：訊息過長，超過緩衝區上限 (256 bytes)");
    return;
  }

  // 2. 準備明文 Buffer
  uint8_t *plaintext = (uint8_t*) malloc(storedLength);
  memset(plaintext, 0, storedLength); // Zero Padding
  message.getBytes(plaintext, storedLength + 1);

  // 3. 初始化硬體加密器
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_enc(&aes, encryptionKey, 128); // 注意：這裡是 setkey_enc

  unsigned long start = micros();

  // 4. 執行加密 (Block by Block)
  for (int i = 0; i < storedLength; i += 16) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, plaintext + i, storedCiphertext + i);
  }
  
  unsigned long end = micros();
  mbedtls_aes_free(&aes);
  free(plaintext);

  // 5. 印出結果
  Serial.println("\n----------------- [加密階段] -----------------");
  Serial.printf("使用金鑰 (Hex): ");
  for(int i=0; i<16; i++) Serial.printf("%02X ", encryptionKey[i]);
  Serial.println();
  
  Serial.printf("原始訊息: %s\n", message.c_str());
  Serial.printf("加密耗時: %lu us\n", end - start);
  Serial.print("密文 (Hex): ");
  for (int i = 0; i < storedLength; i++) {
    Serial.printf("%02X ", storedCiphertext[i]);
  }
  Serial.println("\n----------------------------------------------");
  Serial.println(">>> 密文已暫存。");
  Serial.println(">>> 請輸入「解密金鑰」來嘗試還原訊息 ...");
}

// =========================================================
// 階段 3: 使用解密 Key 還原訊息
// =========================================================
void performDecryption() {
  // 1. 準備還原用的 Buffer
  uint8_t *decryptedText = (uint8_t*) malloc(storedLength + 1); // +1 放結尾符號
  memset(decryptedText, 0, storedLength + 1);

  // 2. 初始化硬體解密器
  mbedtls_aes_init(&aes);
  // 【重要】解密時必須呼叫 setkey_dec，因為 AES 解密的 Key Schedule 運算不同
  mbedtls_aes_setkey_dec(&aes, decryptionKey, 128); 

  unsigned long start = micros();

  // 3. 執行解密 (Block by Block)
  for (int i = 0; i < storedLength; i += 16) {
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, storedCiphertext + i, decryptedText + i);
  }

  unsigned long end = micros();
  mbedtls_aes_free(&aes);

  // 4. 印出結果
  Serial.println("\n----------------- [解密階段] -----------------");
  Serial.printf("使用金鑰 (Hex): ");
  for(int i=0; i<16; i++) Serial.printf("%02X ", decryptionKey[i]);
  Serial.println();

  Serial.printf("解密耗時: %lu us\n", end - start);
  
  // 比較 Key 是否相同 (僅供教學顯示用)
  bool keyMatch = (memcmp(encryptionKey, decryptionKey, 16) == 0);
  if (keyMatch) {
    Serial.println("[系統偵測] 金鑰正確！");
  } else {
    Serial.println("[系統偵測] 金鑰不符！預期會看到亂碼...");
  }

  Serial.print("還原訊息 (String): [");
  // 直接印出 char*，如果是亂碼可能會印出怪怪的符號
  Serial.print((char*)decryptedText); 
  Serial.println("]");

  // 同時印出 Hex 以便觀察亂碼的真面目
  Serial.print("還原數據 (Hex):    ");
  for(int i=0; i<storedLength; i++) Serial.printf("%02X ", decryptedText[i]);
  Serial.println("\n----------------------------------------------");
  
  free(decryptedText);
  Serial.println("\n>>> 流程結束。請輸入新的「加密金鑰」開始新回合...");
}

// =========================================================
// Setup
// =========================================================
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\n\n=== ESP32 AES 加解密實驗室 ===");
  Serial.println(">>> 步驟 1: 請輸入「加密金鑰 (Encryption Key)」...");
}

// =========================================================
// Loop (狀態機處理)
// =========================================================
void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else if (inChar != '\r') {
      inputBuffer += inChar;
    }
  }

  if (stringComplete) {
    inputBuffer.trim(); // 去除前後空白
    
    // 如果使用者直接按 Enter 沒輸入，忽略
    if (inputBuffer.length() > 0) {
      
      switch (currentState) {
        case WAIT_ENC_KEY:
          parseKey(inputBuffer, encryptionKey);
          Serial.print("加密金鑰已設定: ");
          Serial.println(inputBuffer);
          Serial.println(">>> 步驟 2: 請輸入要加密的「訊息 (Message)」...");
          currentState = WAIT_MSG;
          break;

        case WAIT_MSG:
          performEncryption(inputBuffer);
          currentState = WAIT_DEC_KEY;
          break;

        case WAIT_DEC_KEY:
          parseKey(inputBuffer, decryptionKey);
          performDecryption(); // 解密並印出結果
          currentState = WAIT_ENC_KEY; // 重置流程
          break;
      }
    }
    
    inputBuffer = "";
    stringComplete = false;
  }
}
