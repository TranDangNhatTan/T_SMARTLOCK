#ifndef FUZZY_LOGIC_H
#define FUZZY_LOGIC_H

#include <Arduino.h>

// Khai báo nguyên mẫu hàm tính toán thời gian khóa dựa trên thuật toán Fuzzy Logic
// Hàm này sẽ nhận vào số lần sai và khoảng cách thời gian để tính ra mili-giây phạt
unsigned long calculateFuzzyTimeout(int wrongs, unsigned long timeDiff);

#endif