#include "firebase_ctrl.h"
#include "config.h" // Để lấy MASTER_PASS gốc
#include <time.h>
#include <Preferences.h>
#include <tele_ctrl.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "wifi_ctrl.h"

#define DATABASE_URL "t-smartlock-default-rtdb.firebaseio.com"

FirebaseData fbdo_stream;
FirebaseData fbdo_action;
// FirebaseData fbdo_log;
FirebaseAuth auth;
FirebaseConfig config;

// Khởi tạo biến mật khẩu
String currentMasterPass = MASTER_PASS; 
bool isFirstStreamData = true; // Biến để bỏ qua lần callback đầu tiên khi stream bắt đầu

// Hàm lấy giờ Việt Nam
String getTimeString() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return "N/A";
    char timeStringBuff[50];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(timeStringBuff);
}

// Hàm đẩy trạng thái khóa/mở lên App
void firebase_update_status(String state) {
    Firebase.RTDB.setString(&fbdo_action, "/SmartLock/Status/doorState", state);
    if(state == "LOCKED") {
        Firebase.RTDB.setBool(&fbdo_action, "/SmartLock/Control/isUnlocked", false);
    }
}

// Hàm đẩy lịch sử ra vào
void firebase_log(String method, String userName) {
    FirebaseJson json;
    json.set("method", method);
    json.set("timestamp", getTimeString());
    json.set("userName", userName);
    Firebase.RTDB.pushJSON(&fbdo_action, "/SmartLock/Logs", &json);
}

// Hàm callback: Liên tục lắng nghe khi bạn bấm nút "MỞ KHÓA" trên App
void streamCallback(FirebaseStream data) {
    // 1. Chặn bản chụp dữ liệu rác lúc vừa boot máy
    if (isFirstStreamData) {
        isFirstStreamData = false;
        return; 
    }

    String path = data.dataPath(); // Trả về "/isUnlocked" hoặc "/newPin"

    // 2. Xử lý lệnh MỞ CỬA TỪ APP
    if (path == "/isUnlocked") {
        if (data.dataType() == "boolean" && data.boolData() == true) {
            Serial.println("=> APP GỬI LỆNH MỞ CỬA TỪ XA!");
            accessGranted("Điện thoại (App)"); 
        }
    }
    
    // 3. Xử lý lệnh ĐỔI MÃ PIN TỪ APP
    if (path == "/newPin") {
        if (data.dataType() == "string") {
            String remotePin = data.stringData();
            if (remotePin != "" && remotePin != currentMasterPass) {
                currentMasterPass = remotePin;
                Serial.println(">>> Đã cập nhật mã PIN mới từ App: " + currentMasterPass);

                // 1. Ghi mật mã mới vào bộ nhớ Flash chống mất điện
                Preferences preferences;
                preferences.begin("smartlock", false); // Mở ở chế độ ghi
                preferences.putString("pin", currentMasterPass);
                preferences.end();
                Serial.println("[Flash Memory] Đã lưu mật mã mới vào Flash an toàn!");

                // 2. Gửi thông báo về Telegram ngay lập tức
                tele_send_message("🔐 [Cập nhật] Mật mã bàn phím đã được thay đổi từ xa thành: " + currentMasterPass);
            }
        }
    }
}

void streamTimeoutCallback(bool timeout) {}

// Hàm khởi động Firebase (Chạy 1 lần ở setup)
void firebase_init() {
    Preferences preferences;
    preferences.begin("smartlock", true); // Mở phân vùng "smartlock" ở chế độ chỉ đọc
    currentMasterPass = preferences.getString("pin", MASTER_PASS); // Nếu chưa có pass lưu trong Flash thì dùng MASTER_PASS gốc
    preferences.end();
    Serial.println("[Flash Memory] Đã nạp mật mã hoạt động từ Flash: " + currentMasterPass);

    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // Đồng bộ giờ VN
    config.database_url = DATABASE_URL;
    config.signer.test_mode = true;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    Firebase.RTDB.beginStream(&fbdo_stream, "/SmartLock/Control");
    Firebase.RTDB.setStreamCallback(&fbdo_stream, streamCallback, streamTimeoutCallback);

    if (is_wifi_connected()) {
    firebase_update_status("LOCKED");
    }
}

// Hàm kiểm tra nếu bạn đổi mã PIN trên App (Chạy liên tục ở loop)
void firebase_check_pin() {
    // if (Firebase.ready()) {
    //     if (Firebase.RTDB.getString(&fbdo_action, "/SmartLock/Control/newPin")) {
    //         String remotePin = fbdo_action.stringData();
    //         if (remotePin != "" && remotePin != currentMasterPass) {
    //             currentMasterPass = remotePin;
    //             Serial.println("Đã cập nhật mã PIN mới từ App: " + currentMasterPass);
    //         }
    //     }
    // }
}