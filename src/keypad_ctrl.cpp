#include "keypad_ctrl.h"
#include "config.h"
#include <Keypad.h>

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad = Keypad(makeKeymap(keys), (byte*)PIN_ROWS, (byte*)PIN_COLUMN, ROW_NUM, COLUMN_NUM);

void keypad_init() {
  Serial.println("Bàn phím: Sẵn sàng!");
}

char keypad_read() {
  return keypad.getKey(); // Trả về ký tự nếu có phím được bấm
}