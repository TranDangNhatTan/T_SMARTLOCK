#include <Arduino.h>
#include "config.h"
#include "audio_ctrl.h"
#include "lock_ctrl.h"
#include "keypad_ctrl.h"
#include "rfid_ctrl.h"
#include "finger_ctrl.h"
#include "tele_ctrl.h"
#include "firebase_ctrl.h"
#include "fuzzy_logic.h"
#include "wifi_ctrl.h"

String inputPassword = "";
int wrongCount = 0;

// Thêm 3 biến quản lý thời gian khóa mạch
unsigned long lastFailTime = 0;   
unsigned long lockoutUntil = 0;   
bool isSystemLocked = false;

// Thêm biến này cùng với các biến quản lý thời gian khóa mạch
unsigned long systemReadyTime = 0;

// >>> THÊM 2 BIẾN NÀY ĐỂ ĐIỀU KHIỂN CÒI BÁO ĐỘNG <<<
bool isThiefAlarm = false;       // Cờ đánh dấu có phải trộm hay không
unsigned long beepStartTime = 0; // Mốc thời gian bắt đầu được phép kêu tiếng bíp

// Hàm xử lý mở cửa thành công
void accessGranted(String method) {  
  Serial.println("\n>>> TRUY CẬP ĐƯỢC PHÉP <<<");
  wrongCount = 0; // Reset bộ đếm sai
  
  if(is_wifi_connected()) {
  firebase_update_status("UNLOCKED"); // Cập nhật trạng thái mở cửa lên Firebase
  delay(200); // Đợi trạng thái được cập nhật trước khi ghi log
  firebase_log(method, "Người nhà"); // Ghi log vào Firebase
  delay(200); // Đợi log được ghi trước khi gửi thông báo
  // Gửi thông báo đến điện thoại
  tele_send_message("🔓 Cửa đã được mở bằng: " + method);
  }else {
    Serial.println("⚠️ Không có kết nối Wi-Fi. Bỏ qua đồng bộ, mở cửa ngay");
  }
  audio_play(3); // "Xác thực thành công. Mời bạn vào nhà."
  lock_open();   // Mở rơ-le khóa cửa
  inputPassword = ""; // Xóa mật khẩu đang nhập dở

  delay(3000);
  if (is_wifi_connected()) {
  firebase_update_status("LOCKED"); // Tự động khóa lại sau 3 giây
  } else {
    Serial.println("⚠️ Không có kết nối Wi-Fi. Bỏ qua đồng bộ, khóa cửa ngay");
  }
}

// Hàm xử lý khi xác thực thất bại
void accessDenied() {
  Serial.println("\n>>> TRUY CẬP BỊ TỪ CHỐI <<<");
  wrongCount++;
  // 1. Tính toán khoảng thời gian giữa 2 lần nhập sai
  unsigned long now = millis();
unsigned long timeDiff = now - systemReadyTime; // Chỉ đo thời gian từ lúc mạch sẵn sàng  

  // 2. Gọi bộ não Fuzzy Logic để lấy án phạt
  unsigned long penaltyTime = calculateFuzzyTimeout(wrongCount, timeDiff);
  
  // 3. Đặt mốc thời gian hệ thống bị phong tỏa
  lockoutUntil = millis() + penaltyTime;

  // 4. Thực thi hình phạt
  if (penaltyTime >= 55000) { 
    // Trộm phá khóa: Phạt nặng nhất
    isThiefAlarm = true; // Bật cờ xác nhận trộm
    
    // Đặt lịch: Đợi 4000ms (4 giây) cho máy đọc xong câu cảnh báo rồi mới bắt đầu bíp
    beepStartTime = millis() + 4000;
    audio_play(5); 
    delay(100); 
    if (is_wifi_connected()) {
    // Gửi cảnh báo (Có delay 200ms để chống sập SSL)
    firebase_update_status("EMERGENCY_LOCKED");
    delay(200);
    firebase_log("Xâm nhập trái phép", "CẢNH BÁO TRỘM");
    delay(200);
    tele_send_message("🚨🚨 CẢNH BÁO NGUY HIỂM: Phát hiện hành vi dò khóa liên tục! Hệ thống đã kích hoạt chế độ PHONG TỎA TOÀN DIỆN trong 1 phút.");
    } else {
      Serial.println("⚠️ Không có kết nối Wi-Fi. Bỏ qua đồng bộ, kích hoạt báo động ngay");
    }
    // Chụp ảnh kẻ gian
    digitalWrite(CAM_TRIGGER_PIN, HIGH); 
    delay(1000);
    digitalWrite(CAM_TRIGGER_PIN, LOW);

    wrongCount = 0; 
  } 
  else {
    // Người nhà nhập sai: Cảnh báo nhẹ nhàng
    isThiefAlarm = false; // Reset cờ trộm nếu chỉ là người nhà nhập sai
    audio_play(4); 
    delay(100);
  }

  inputPassword = ""; // Xóa sạch pass đang nhập dở
}

void setup() {
  Serial.begin(115200);
  
  // Cấu hình chân kích hoạt Camera 
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
  wifi_init(); // Khởi tạo Wi-Fi

  delay(500); // Đợi các thiết bị ổn định trước khi kết nối Firebase
  firebase_init();
  if(is_wifi_connected()){
  firebase_update_status("LOCKED"); // Đảm bảo trạng thái ban đầu là khóa
  }
  delay(1000);

  Serial.println("\n--- HỆ THỐNG ĐÃ SẴN SÀNG ---");
  audio_play(1); // Phát lời chào
  if (is_wifi_connected()) {
    tele_send_message("✅ Hệ thống Smart Lock đã khởi động thành công!");
  }
  
}

void loop() {

  // --- BẮT ĐẦU ĐOẠN CHÈN THÊM: CHỐT CHẶN BẢO MẬT ---
  if (millis() < lockoutUntil) {
    if (!isSystemLocked) {
      isSystemLocked = true;
      if (is_wifi_connected()) firebase_update_status("LOCKED_TIMEOUT"); 
      Serial.println("❌ HỆ THỐNG ĐANG BỊ PHONG TỎA! TỪ CHỐI MỌI PHƯƠNG THỨC XÁC THỰC VẬT LÝ...");
    }
    // >>> ĐIỀU KIỆN MỚI: Chỉ kêu khi LÀ TRỘM và ĐÃ ĐỌC XONG CẢNH BÁO <<<
    if (isThiefAlarm == true && millis() >= beepStartTime) {
      static unsigned long lastBeep = 0;
      if (millis() - lastBeep > 500) { 
        audio_play(9);                 
        lastBeep = millis();           
      }
    }
    
    firebase_check_pin(); // Dù mạch bị liệt nhưng vẫn cho phép App Firebase gọi lệnh mở cửa cứu hộ
    return; // Dội ngược luồng chạy, chặn đứng mọi cảm biến bên dưới
  }

  // Tự động giải phóng hệ thống khi hết phạt
  if (isSystemLocked) {
    isSystemLocked = false;
    isThiefAlarm = false; // Reset cờ trộm khi hết phạt
    systemReadyTime = millis(); // Đặt lại mốc thời gian sẵn sàng cho lần tính toán tiếp theo
    if (is_wifi_connected()) {
    firebase_update_status("LOCKED"); 
    tele_send_message("🔓 Hệ thống Smart Lock đã hết thời gian phạt. Bàn phím, Thẻ từ và Vân tay đã hoạt động bình thường trở lại.");
    
    }
    Serial.println("✅ HỆ THỐNG ĐĐƯỢC GIẢI PHÓNG! Các phương thức mở đã có thể sử dụng lại.");
    audio_play(1); // Tiếng bíp khởi động lại
  }

  // 1. KIỂM TRA BÀN PHÍM MA TRẬN
  char key = keypad_read();
  if (key) {
    audio_play(9); // Phát âm thanh bấm phím
    if (key == '#') {
      if (inputPassword == currentMasterPass) {
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
  // TRẠM GÁC MẠNG TỰ ĐỘNG
wifi_maintain(); // Tự động xử lý nếu rớt mạng

if (is_wifi_connected()) {
    firebase_check_pin(); // Chỉ kiểm tra Firebase khi có mạng
}


}