#include "finger_ctrl.h"
#include "config.h"
#include <Adafruit_Fingerprint.h>

// Sử dụng cổng Serial số 2 của ESP32
HardwareSerial mySerial2(2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial2);

void finger_init() {
  mySerial2.begin(57600, SERIAL_8N1, FINGER_RX, FINGER_TX);
  delay(100);
  
  if (finger.verifyPassword()) {
    Serial.println("Vân tay AS608: Sẵn sàng!");
  } else {
    Serial.println("LỖI: Không tìm thấy module Vân tay!");
  }
}

// Trả về ID vân tay nếu đúng, trả về -1 nếu sai hoặc không có ai chạm
int finger_read() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    return finger.fingerID; // Vân tay hợp lệ, trả về ID (1, 2, 3...)
  }
  return -1; // Vân tay sai
}