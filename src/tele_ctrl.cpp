#include "tele_ctrl.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void tele_init() {
  Serial.print("Đang kết nối WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Đợi kết nối WiFi tối đa 10 giây (20 lần x 500ms), không kết nối được thì chạy offline
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[OK] WiFi đã kết nối!");
    // Bắt buộc cho ESP32 khi dùng UniversalTelegramBot
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  } else {
    Serial.println("\n[CẢNH BÁO] Không có WiFi. Hệ thống sẽ chạy Offline!");
  }
}

void tele_send_message(String message) {
  // Chỉ gửi tin nhắn nếu có mạng
  if (WiFi.status() == WL_CONNECTED) {
    bot.sendMessage(CHAT_ID, message, "");
    Serial.println("Đã gửi Telegram: " + message);
  } else {
    Serial.println("Rớt mạng! Bỏ qua gửi Telegram.");
  }
}