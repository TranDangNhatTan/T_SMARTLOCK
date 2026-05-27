#ifndef WIFI_CTRL_H
#define WIFI_CTRL_H

#include <Arduino.h>
#include <WiFi.h>

// Khởi tạo Wi-Fi ở chế độ chạy nền
void wifi_init();

// Hàm tuần tra mạng, tự động kết nối lại nếu rớt mạng (gọi trong loop)
void wifi_maintain();

// Hàm kiểm tra trạng thái mạng để quyết định có gửi log/telegram hay không
bool is_wifi_connected();

#endif