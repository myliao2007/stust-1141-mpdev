/*
 * ESP32-S3 壓力感測調光燈 (Pressure Sensitive Light)
 * 原理：將觸控數值線性映射 (Map) 到 亮度
 * 範圍：30,000 (滅) <---> 100,000 (全亮)
 */

const int TOUCH_PIN = 4;
const int RGB_PIN = 48; 

// === 區間設定 ===
const int MIN_VAL = 30000;  // 起始門檻 (低於此值不亮)
const int MAX_VAL = 100000; // 最大門檻 (高於此值最亮)
const int MAX_PWM = 255;    // PWM 最大值 (全亮)

void setup() {
  Serial.begin(115200);
  neopixelWrite(RGB_PIN, 0, 0, 0); // 初始關燈
  Serial.println("System Ready. Touch lightly to start dimming...");
}

void loop() {
  // 1. 讀取原始數值
  int raw = (int)touchRead(TOUCH_PIN);
  int brightness = 0; // 最終要輸出的亮度

  // 2. 區間判斷與映射 (核心邏輯)
  if (raw <= MIN_VAL) {
    // --- 情況 A：低於 3萬 (沒摸或雜訊) ---
    brightness = 0; 
  } 
  else if (raw >= MAX_VAL) {
    // --- 情況 B：高於 10萬 (飽和) ---
    brightness = MAX_PWM; 
  } 
  else {
    // --- 情況 C：在 3萬 ~ 10萬 之間 (線性調光) ---
    // 使用 map() 函式將數值等比例轉換
    // map(數值, 來源下限, 來源上限, 目標下限, 目標上限)
    brightness = map(raw, MIN_VAL, MAX_VAL, 0, MAX_PWM);
  }

  // 3. 輸出 LED
  // 這裡控制綠色通道 (Green)，您也可以改成別的顏色
  neopixelWrite(RGB_PIN, 0, brightness, 0);

  // 4. 監控數值 (方便教學與除錯)
  // 格式：原始值 -> 轉換後的亮度
  Serial.print("Raw: ");
  Serial.print(raw);
  Serial.print("  ==>  Brightness: ");
  Serial.println(brightness);

  delay(20); // 保持更新頻率
}
