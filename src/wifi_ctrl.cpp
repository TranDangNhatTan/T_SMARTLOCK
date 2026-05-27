#include "wifi_ctrl.h"
#include "config.h" // Kéo file config vào để lấy thông tin mạng

unsigned long lastWifiCheckTime = 0;
const unsigned long wifiCheckInterval = 10000; // 10 giây kiểm tra một lần

void wifi_init() {
    Serial.print("Đang khởi động module Wi-Fi (Chạy nền): ");
    Serial.println(WIFI_SSID); // Thay bằng biến SSID thực tế 
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Thay bằng biến PASS thực tế 
    
    // TUYỆT ĐỐI KHÔNG DÙNG vòng lặp while(WiFi.status() != WL_CONNECTED) ở đây
    // Mạch sẽ tự kết nối ngầm, hàm setup() vẫn chạy bình thường.
}

void wifi_maintain() {
    // Nếu mất mạng và đã qua 10 giây kể từ lần thử trước
    if (WiFi.status() != WL_CONNECTED) {
        if (millis() - lastWifiCheckTime >= wifiCheckInterval) {
            Serial.println("⚠️ Mất kết nối Wi-Fi. Đang tự động kết nối lại...");
            WiFi.disconnect();
            WiFi.reconnect(); // Lệnh này không làm đứng mạch
            lastWifiCheckTime = millis();
        }
    }
}

bool is_wifi_connected() {
    return WiFi.status() == WL_CONNECTED;
}