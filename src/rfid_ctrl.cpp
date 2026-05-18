#include "rfid_ctrl.h"
#include "config.h"
#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);

void rfid_init() {
  SPI.begin(); // Khởi tạo bus SPI
  mfrc522.PCD_Init();
  Serial.println("Thẻ từ RC522: Sẵn sàng!");
}

String rfid_read() {
  // Nếu không có thẻ nào quẹt vào thì thoát luôn
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return "";
  }
  
  // Đọc mã UID của thẻ
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  
  mfrc522.PICC_HaltA(); // Dừng đọc để tránh đọc liên tục 1 thẻ
  return uid;
}