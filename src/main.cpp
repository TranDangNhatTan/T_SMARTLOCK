#include <Arduino.h>
#include "config.h"
#include "audio_ctrl.h"
#include "lock_ctrl.h"
#include "keypad_ctrl.h"
#include "rfid_ctrl.h"
#include "finger_ctrl.h"
#include "tele_ctrl.h"

String inputPassword = "";
int wrongCount = 0;

// Hàm xử lý mở cửa thành công
void accessGranted(String method) {  
  Serial.println("\n>>> TRUY CẬP ĐƯỢC PHÉP <<<");
  wrongCount = 0; // Reset bộ đếm sai
  
  // Gửi thông báo đến điện thoại
  tele_send_message("🔓 Cửa đã được mở bằng: " + method);
  
  audio_play(3); // "Xác thực thành công. Mời bạn vào nhà."
  lock_open();   // Mở rơ-le khóa cửa
  inputPassword = ""; // Xóa mật khẩu đang nhập dở
}

// Hàm xử lý khi xác thực thất bại
void accessDenied() {
  Serial.println("\n>>> TRUY CẬP BỊ TỪ CHỐI <<<");
  wrongCount++;
  audio_play(4); // "Thông tin không chính xác."
  
  if (wrongCount >= 3) {
    Serial.println("!!! CẢNH BÁO: XÂM NHẬP TRÁI PHÉP !!!");
    audio_play(5); // "Cảnh báo. Hệ thống bị khóa."
    
    // Gửi Telegram báo động
    tele_send_message("🚨 CẢNH BÁO: Phát hiện xâm nhập! Nhập sai 3 lần liên tiếp.");
    
    // Đánh thức ESP32-CAM chụp ảnh
    digitalWrite(CAM_TRIGGER_PIN, HIGH); 
    delay(1000);
    digitalWrite(CAM_TRIGGER_PIN, LOW);
    
    wrongCount = 0; // Reset lại bộ đếm sau khi đã báo động
  }
  inputPassword = ""; // Xóa mật khẩu đang nhập dở
}

void setup() {
  Serial.begin(115200);
  
  // Cấu hình chân kích hoạt Camera ngay từ đầu để tránh lỗi chập chờn
  pinMode(CAM_TRIGGER_PIN, OUTPUT);
  digitalWrite(CAM_TRIGGER_PIN, LOW);

  Serial.println("\n--- ĐANG KHỞI ĐỘNG HỆ THỐNG SMART LOCK ---");

  // Khởi tạo toàn bộ linh kiện
  audio_init();
  lock_init();
  keypad_init();
  rfid_init();
  finger_init();
  tele_init();

  Serial.println("\n--- HỆ THỐNG ĐÃ SẴN SÀNG ---");
  audio_play(1); // Phát lời chào
  tele_send_message("✅ Hệ thống Smart Lock đã khởi động thành công!");
}

void loop() {
  // 1. KIỂM TRA BÀN PHÍM MA TRẬN
  char key = keypad_read();
  if (key) {
    if (key == '#') {
      if (inputPassword == MASTER_PASS) {
        accessGranted("Mật mã bàn phím");
      } else {
        Serial.println("Sai mật khẩu!");
        accessDenied();
      }
    } else if (key == '*') {
      inputPassword = "";
      Serial.println("\n[Đã xóa mật mã đang nhập]");
    } else {
      inputPassword += key;
      Serial.print("*"); // Bảo mật: Chỉ in dấu sao ra màn hình
    }
  }

  // 2. KIỂM TRA THẺ TỪ RFID
  String uid = rfid_read();
  if (uid != "") {
    Serial.print("Đang kiểm tra thẻ UID: ");
    Serial.println(uid);
    
    // Kiểm tra 2 thẻ hợp lệ của Tân
    if (uid == "81180054" || uid == "24C9FD04") { 
      accessGranted("Thẻ từ RFID");
    } else {
      accessDenied();
    }
  }

  // 3. KIỂM TRA VÂN TAY (ĐÃ VÁ LỖI BẢO MẬT)
  int fingerID = finger_read();
  if (fingerID != -1) { // Có người chạm tay vào
    if (fingerID == -2) {
      Serial.println("Phát hiện vân tay LẠ!");
      accessDenied();
    } else {
      Serial.print("Tìm thấy vân tay hợp lệ, ID: ");
      Serial.println(fingerID);
      accessGranted("Vân tay (ID: " + String(fingerID) + ")");
    }
  }
}