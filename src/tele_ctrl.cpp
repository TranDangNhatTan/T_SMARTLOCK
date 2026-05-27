#include "tele_ctrl.h"
#include "config.h"
#include "wifi_ctrl.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void tele_init() {
  Serial.println("--- Khởi tạo module Telegram ---");
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  
  Serial.println("[OK] Đã nạp chứng chỉ bảo mật Telegram.");
}

void tele_send_message(String message) {
  // Chỉ gửi tin nhắn nếu có mạng
  if (is_wifi_connected()) {
    bot.sendMessage(CHAT_ID, message, "");
    Serial.println("Đã gửi Telegram: " + message);
  } else {
    Serial.println("Mạng OFFLINE! Bỏ qua gửi Telegram.");
  }
}