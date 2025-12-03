const int touchPin = 4; 
unsigned long startTime;
bool isRecording = false;
bool isFinished = false;

void setup() {
  Serial.begin(115200);
  
  // 給一點緩衝時間準備，例如 3 秒
  // 在這邊提醒：「手指準備好動作...」
  delay(3000); 
  
  Serial.println("START"); // Excel 複製時要避開這行
  
  // 紀錄開始的時間點
  startTime = millis();
  isRecording = true;
}

void loop() {
  // 如果已經結束，就什麼都不做，卡在這裡
  if (isFinished) {
    delay(500);
    return;
  }

  // 檢查是否還在 10 秒內 (10000 毫秒)
  if (millis() - startTime <= 10000) {
    // 輸出數據
    Serial.println(touchRead(touchPin));
    
    // 保持採樣頻率 50Hz
    delay(20);
  } else {
    // 時間到，標記結束
    isFinished = true;
    isRecording = false;
    Serial.println("END"); // 提示結束了
  }
}
