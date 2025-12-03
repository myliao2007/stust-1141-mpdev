// 這是觀察波形用的範例，請打開「工具 > 序列繪圖家(Serial Plotter)」

const int touchPin = 4; 

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // 讀取數值
  int val = touchRead(touchPin);
  
  // 輸出格式：變數名稱:數值 (這是 Arduino 序列繪圖家的標準格式)
  Serial.print("Touch:");
  Serial.print(val);
  Serial.print(",");
  // 畫一條參考線，方便觀察
  Serial.print("Threshold:");
  Serial.println(40000); 
  
  delay(20); // 取樣頻率約 50Hz
}
