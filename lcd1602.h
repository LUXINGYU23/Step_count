/*函数功能说明
    * lcd1602_init() 初始化LCD1602
    * lcd1602_display_switch() 设置LCD1602显示状态
    * lcd1602_cursor_shift() 设置光标移动方向
    * lcd1602_clear() 清空LCD1602显示
    * lcd1602_cursor_return() 光标返回到起始位置
    * lcd1602_char_set() 设置LCD1602指定位置字符
    * lcd1602_string_set() 设置LCD1602指定位置字符串
*/
#ifndef LCD1602_H
#define LCD1602_H

#include <reg51.h>
#include "types.h"

#define true 1
#define false 0

// LCD1602 控制引脚
sbit LCD1602_RS = P2 ^ 7;
sbit LCD1602_RW = P1 ^ 2;
sbit LCD1602_E = P2 ^ 6;
#define LCD1602_PORT P0

// 函数声明
void lcd1602_init();
void lcd1602_display_switch(bool p_show_char, bool p_show_cursor, bool p_flash_cursor);
void lcd1602_cursor_shift(bool p_right);
void lcd1602_clear();
void lcd1602_cursor_return();
void lcd1602_char_set(uint8 p_x, uint8 p_y, char p_char);
void lcd1602_string_set(uint8 p_x, uint8 p_y, const char* p_string);

#endif // LCD1602_H