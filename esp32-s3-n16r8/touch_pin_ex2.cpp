const int touchPin = 4; 

void setup() {
  Serial.begin(115200);
}

void loop() {
  // 輸出純數字，中間用逗號隔開(如果有兩個感測器)，最後換行
  // 這裡我們只有一個特徵
  Serial.println(touchRead(touchPin));
  
  // 重要：保持固定的採樣頻率 (Frequency)
  // 20ms delay = 50Hz
  delay(20);
}
