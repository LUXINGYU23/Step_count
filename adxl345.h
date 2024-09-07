/*
函数功能说明
ADXL345_WriteReg(uint8 RegAddress, uint8 Data)：写入ADXL345寄存器
ADXL345_ReadReg(uint8 RegAddress)：读取ADXL345寄存器
ADXL345_ReadReg_Get_Data(short *x, short *y, short *z)：读取ADXL345三轴数据
ADXL345_Init(void)：初始化ADXL345
filter_calculate(filter_avg_t *filter, axis_info_t *sample,peak_value_t *peak,int32 ccnt)：滤波计算
cnt_update(peak_value_t *peak, axis_info_t *cur_sample,slid_reg_t *slid,int32 current_cnt)：更新步数
detect_step(peak_value_t *peak, slid_reg_t *slid, axis_info_t *cur_sample,int32 current_cnt)：检测步数
display_x_y_z()：显示三轴数据
send_packet(uint8 *packet, uint8 length)：发送数据包
lcd_step_display(int32 cnt)：显示步数
*/
#ifndef ADXL345_H
#define ADXL345_H

#include <reg51.h>
#include "types.h"
#include "i2c.h"
#include"lcd1602.h"
#include "buffered_uart.h"

// ADXL345地址
#define ADXL345_ADDR 0x53

// 滤波器样本数
#define FILTER_CNT 4

// 宏定义
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define SAMPLE_SIZE  50 // 样本数
#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
#define DYNAMIC_PRECISION  50 // 动态精度
#define MOST_ACTIVE_NULL 0 // 未找到最活跃轴
#define MOST_ACTIVE_X 1 // 最活跃轴X
#define MOST_ACTIVE_Y 2 // 最活跃轴Y
#define MOST_ACTIVE_Z 3 // 最活跃轴Z
#define ACTIVE_PRECISION 40 // 活跃轴最小变化值
#define SEARCH_MODE 0
#define CONFIRM_MODE 1

//#define TIME_RESET_THRESHOLD 1000 // 定期重置时间的阈值
// 三轴数据结构体
typedef struct {
    short x;
    short y;
    short z;
} axis_info_t;

// 滤波器均值结构体
typedef struct {
    axis_info_t info[FILTER_CNT];
} filter_avg_t;

// 最大最小值结构体
typedef struct {
    axis_info_t newmax;
    axis_info_t newmin;
    axis_info_t oldmax;
    axis_info_t oldmin;
} peak_value_t;

// 线性移位寄存器结构体
typedef struct {
    axis_info_t new_sample;
    axis_info_t old_sample;
} slid_reg_t;

// 函数声明
void ADXL345_WriteReg(uint8 RegAddress, uint8 Data);
uint8 ADXL345_ReadReg(uint8 RegAddress);
void ADXL345_ReadReg_Get_Data(short *x, short *y, short *z);
void ADXL345_Init(void);
void filter_calculate(filter_avg_t *filter, axis_info_t *sample,peak_value_t *peak,int32 ccnt);
int32 cnt_update(peak_value_t *peak, axis_info_t *cur_sample,slid_reg_t *slid,int32 current_cnt);
int8 detect_step(peak_value_t *peak, slid_reg_t *slid, axis_info_t *cur_sample,int32 current_cnt);
void display_x_y_z();
void send_packet(uint8 *packet, uint8 length);
void lcd_step_display(int32 cnt);
#endif // ADXL345_H