#include "audio_ctrl.h"
#include "config.h"
#include <DFRobotDFPlayerMini.h>

HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;

void audio_init() {
  // Khởi tạo cổng Serial số 1 để giao tiếp với DFPlayer
  // Theo config.h: DF_TX_PIN (34) là RX của ESP32, DF_RX_PIN (22) là TX của ESP32
  mySerial.begin(9600, SERIAL_8N1, DF_TX_PIN, DF_RX_PIN);
  
  Serial.println("Đang khởi tạo module Âm thanh DFPlayer...");
  
  if (myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer: Sẵn sàng!");
    myDFPlayer.volume(30); // Đặt mức âm lượng tối đa
    delay(500); // Chờ một chút cho module ổn định
  } else {
    Serial.println("LỖI: Không tìm thấy DFPlayer. Hãy kiểm tra dây RX/TX và thẻ nhớ!");
  }
}

void audio_play(int file_number) {
  myDFPlayer.play(file_number);
  // Thêm delay nhỏ để tránh việc bị gọi đè file liên tục
  delay(100); 
}