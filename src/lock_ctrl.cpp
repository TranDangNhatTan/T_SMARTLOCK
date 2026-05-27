#include "lock_ctrl.h"
#include "config.h"

void lock_init() {
  pinMode(RELAY_PIN, OUTPUT);
  // Khởi động mạch, giữ chốt ở trạng thái đóng (Mức LOW)
  digitalWrite(RELAY_PIN, HIGH); 
  Serial.println("Relay khóa cửa: Sẵn sàng!");
}

void lock_open() {
  Serial.println("Trạng thái: Đang mở khóa...");
  // Cấp điện cho Relay để rút chốt vào (mở cửa)
  digitalWrite(RELAY_PIN, LOW); 
  
  // Giữ cửa mở trong 5 giây cho người dùng bước vào
  delay(5000); 
  
  // Ngắt điện, đẩy chốt ra khóa lại
  digitalWrite(RELAY_PIN, HIGH);  
  Serial.println("Trạng thái: Đã tự động khóa lại.");
}