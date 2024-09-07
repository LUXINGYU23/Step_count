/*
本文件定义了i2c总线操作的函数：

向指定设备地址的指定寄存器写入指定的值
void i2c_write(uint8 p_address,uint8 p_register,uint8 p_data)

从指定设备地址的指定寄存器中读取值，返回读取的值
uint8 i2c_read(uint8 p_address,uint8 p_register);

向指定设备地址的指定寄存器开始，连续写入指定长度的数据
void i2c_multi_write(uint8 p_address,uint8 p_register_start,uint8 p_count,uint8* p_buffer);

从指定设备地址的指定寄存器开始，连续读出指定长度的数据
void i2c_multi_read(uint8 p_address,uint8 p_register_start,uint8 p_count,uint8* p_buffer);

如果要修改SCL和SDA的引脚，请到i2c.c中修改I2C_SCL和I2C_SDA宏
如果要修改I2C的速率，请到i2c.c中修改I2C_DELAY宏

*/
#ifndef I2C_H
#define I2C_H

#include "types.h"

void i2c_write(uint8 p_address,uint8 p_register,uint8 p_data);

uint8 i2c_read(uint8 p_address,uint8 p_register);

void i2c_multi_write(uint8 p_address,uint8 p_register_start,uint8 p_count,uint8* p_buffer);

void i2c_multi_read(uint8 p_address,uint8 p_register_start,uint8 p_count,uint8* p_buffer);

#endif