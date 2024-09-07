/*
该文件用来实现一个带有缓冲机制的串口
定义了方法

void uart_init(uint16 p_baud,uint8* p_buffer,uint8 p_capacity);
初始化串口，模式为8-n-1（8数据位，无校验位，1停止位），参数为
波特率、缓存、缓存容量

void uart_close();
关闭串口，释放占用的资源（缓冲区、定时器1）

uint8 uart_available();
获取缓存中以接收但还未读取的字节数量

uint8 uart_read();
从缓存中读取一个字节

bool uart_is_overflow();
判断缓存是否溢出（由于没有及时读取）

void uart_overflow_reset();
清除缓存溢出标志位

void uart_write(uint8 p_data);
发送一个字节

void uart_print(char* p_string);
发送一个字符串

void uart_println(char* p_string);
发送一个字符串并换行

目前只支持常用波特率300、600、1200、2400、4800
*/

#ifndef BUFFERED_UART_H
#define BUFFERED_UART_H

#include "types.h"

void uart_init(uint8* p_buffer,uint8 p_capacity);
void uart_close();
uint8 uart_available();
uint8 uart_read();
bool uart_is_overflow();
void uart_overflow_reset();
void uart_write(uint8 p_data);
void uart_print(char* p_string);
void uart_println(char* p_string);

#endif