/*
 * ESP32-S3 多段觸控示波器 (Multi-Level Touch Monitor)
 * 功能：根據觸控數值大小，顯示 4 種不同顏色的燈號
 * 邏輯：
 * - > 100,000 : 藍燈 (Blue)
 * - > 80,000  : 橘燈 (Orange)
 * - > 40,000  : 綠燈 (Green)
 * - <= 40,000 : 白燈 (White)
 */

const int touchPin = 4;
const int rgbPin = 48;     

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // 初始化：先亮一下紅燈測試，然後關掉
  neopixelWrite(rgbPin, 50, 0, 0);
  delay(500);
  neopixelWrite(rgbPin, 0, 0, 0);
  
  Serial.println("System Ready. Monitoring Touch Value...");
}

void loop() {
  // 1. 讀取數值
  int val = touchRead(touchPin);
  
  // 2. 輸出給序列繪圖家觀察 (Serial Plotter)
  // 為了方便觀察門檻，我們可以在圖表上畫出參考線
  Serial.print("Touch:");
  Serial.print(val);
  Serial.print(",");
  Serial.print("Level_100k:"); Serial.print(100000); Serial.print(",");
  Serial.print("Level_80k:");  Serial.print(80000);  Serial.print(",");
  Serial.print("Level_40k:");  Serial.println(40000);

  // 3. LED 多段顏色控制邏輯
  
  if (val > 100000) {
    // [區間 1] 高於 10 萬 -> 藍燈
    // (紅, 綠, 藍)
    neopixelWrite(rgbPin, 0, 0, 50); 
  } 
  else if (val > 80000) {
    // [區間 2] 高於 8 萬 (且小於等於 10 萬) -> 橘燈
    // 橘色 = 紅多 + 綠少
    neopixelWrite(rgbPin, 50, 25, 0);
  } 
  else if (val > 40000) {
    // [區間 3] 高於 4 萬 (且小於等於 8 萬) -> 綠燈
    neopixelWrite(rgbPin, 0, 50, 0);
  } 
  else {
    // [區間 4] 低於等於 4 萬 -> 白燈
    // 白色 = 紅 + 綠 + 藍 全部都有
    neopixelWrite(rgbPin, 20, 20, 20); // 亮度設低一點，白光很耗電且刺眼
  }
  
  delay(20); // 取樣頻率約 50Hz
}
