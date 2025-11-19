#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// --- 硬體設定：RGB LED ---
// ESP32-S3 DevKitC-1 官方板子的 RGB LED 通常在 GPIO 48
// 如果您的板子不會亮，請嘗試改成 38
#define RGB_LED_PIN 48 

// --- BLE UUID 設定 ---
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// 輔助函式：設定 RGB 顏色
// 注意：neopixelWrite 是 ESP32 Arduino SDK 3.0+ 內建的函式
void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
  // neopixelWrite(腳位, 紅, 綠, 藍)
  neopixelWrite(RGB_LED_PIN, r, g, b);
}

// --- Callback: 處理連線狀態 ---
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println(">> 手機已連線！");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println(">> 手機已斷線...");
    }
};

// --- Callback: 處理手機傳來的資料 (Write) ---
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // [修正] 使用 Arduino String 接收資料，解決 SDK 3.0 編譯錯誤
      String rxValue = pCharacteristic->getValue(); 

      if (rxValue.length() > 0) {
        Serial.print("收到指令: ");
        // 列印收到的字串
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }
        Serial.println();

        // [邏輯] 判斷指令
        // indexOf != -1 代表字串中有包含該指令
        if (rxValue.indexOf("A") != -1) { 
          // 亮橘燈 (Orange)
          // 橘色是 紅+綠。為了避免太刺眼，數值不要設到 255
          setLEDColor(64, 32, 0); 
          Serial.println("動作: 開橘燈");
        }
        else if (rxValue.indexOf("B") != -1) {
          // 關燈 (全部設為 0)
          setLEDColor(0, 0, 0);
          Serial.println("動作: 關燈");
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  
  // 初始化 RGB LED 腳位 (SDK 3.0 不需要 pinMode，但寫了保險)
  pinMode(RGB_LED_PIN, OUTPUT);
  setLEDColor(0, 0, 0); // 一開始先關燈

  // --- [防衝突] 自動產生唯一裝置名稱 ---
  uint64_t chipid = ESP.getEfuseMac(); 
  char devName[32];
  // 取 MAC 位址後兩碼 (例如 A1B2)
  sprintf(devName, "ESP32-S3-%04X", (uint16_t)(chipid >> 32)); 

  Serial.println("------------------------------------");
  Serial.print("【重要】本裝置名稱為: ");
  Serial.println(devName);
  Serial.println("請在 App 中搜尋此名稱進行連線");
  Serial.println("------------------------------------");

  // 1. 初始化 BLE
  BLEDevice::init(devName); 

  // 2. 建立 Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. 建立 Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 4. 建立 RX 特徵 (接收)
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // 5. 建立 TX 特徵 (傳送/通知)
  pTxCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_TX,
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
  pTxCharacteristic->addDescriptor(new BLE2902());

  // 6. 啟動 Service
  pService->start();

  // 7. 開始廣播
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE 廣播啟動完成...");
}

void loop() {
  // 若已連線，每秒發送計數給手機
  if (deviceConnected) {
    static int counter = 0;
    char txString[20]; 
    sprintf(txString, "Count: %d", counter);
    
    pTxCharacteristic->setValue(txString);
    pTxCharacteristic->notify();
    
    // 為了不讓 Serial 刷太快，這裡不印 Log，只閃爍 TX 狀態
    counter++;
    delay(1000); 
  }

  // 斷線重連邏輯
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); 
      pServer->startAdvertising(); 
      Serial.println("重新開始廣播 (Re-advertising)...");
      oldDeviceConnected = deviceConnected;
  }
  
  if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
  }
}
