#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// === BÀN PHÍM MA TRẬN ===
const byte ROW_NUM    = 4; 
const byte COLUMN_NUM = 4; 
const byte PIN_ROWS[ROW_NUM] = {13, 14, 27, 26}; 
const byte PIN_COLUMN[COLUMN_NUM] = {25, 22, 12, 21};

// === MODULE ÂM THANH ===
#define DF_TX_PIN 32 
#define DF_RX_PIN 33

// === RELAY KHÓA CỬA ===
#define RELAY_PIN 4  

// === THẺ TỪ RFID ===
#define SS_PIN  5    
#define RST_PIN 2    

// === MODULE VÂN TAY ===
#define FINGER_RX 16 
#define FINGER_TX 17 

// === GIAO TIẾP VỚI ESP32-CAM ===
#define CAM_TRIGGER_PIN 15 

// === THÔNG TIN MẠNG & TELEGRAM ===
#define WIFI_SSID "NHAT TAN"         // <-- ĐIỀN TÊN WIFI VÀO ĐÂY
#define WIFI_PASSWORD "khongbiet"        // <-- ĐIỀN PASS WIFI VÀO ĐÂY
#define BOT_TOKEN "8591139124:AAG6kuyTAaDCkUufXdhHYws7O643zUZ3d2s"    // <-- ĐIỀN TOKEN CỦA BOT VÀO ĐÂY
#define CHAT_ID "8616248241"                   // <-- ĐIỀN CHAT ID CỦA BẠN VÀO ĐÂY

// Mật khẩu mặc định hệ thống
const String MASTER_PASS = "123456";

#endif