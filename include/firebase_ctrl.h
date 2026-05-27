#ifndef FIREBASE_CTRL_H
#define FIREBASE_CTRL_H

#include <Arduino.h>

// 1. Khai báo biến mật khẩu dùng chung cho cả mạch và Firebase
extern String currentMasterPass;

// 2. Khai báo các hàm Firebase để main.cpp có thể gọi
void firebase_init();
void firebase_update_status(String state);
void firebase_log(String method, String userName);
void firebase_check_pin();

// 3. Cho phép Firebase gọi ngược lại hàm mở cửa ở main.cpp khi bấm nút trên App
extern void accessGranted(String method);

#endif