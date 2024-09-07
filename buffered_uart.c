/*
buffered_uart.h的实现文件
请务必明确单片机的时钟频率，并相应修改CLK_FREQ_100HZ
CLK_FREQ_100HZ是晶振频率的百分之一
*/

#define CLK_FREQ_100HZ 110592

#include "buffered_uart.h"
#include <reg51.h>

#define INTERRUPT_NO 4

typedef struct
{
    uint8* buffer;
    uint8 max;
    uint8 start;
    uint8 len;
}
queue;

static uint8 queue_read(queue* p_queue)
{
    uint8 t_data;
    if(p_queue->len==0)
        return 0;
    t_data=p_queue->buffer[p_queue->start];
    ++p_queue->start;
    --p_queue->len;
    if(p_queue->start==p_queue->max)
        p_queue->start=0;
    return t_data;
}

static void queue_write(queue* p_queue,uint8 p_data)
{
    uint8 t_pos,t_rest;
    if(p_queue->len==p_queue->max)
        return;
    t_rest=p_queue->max-p_queue->len;
    if(p_queue->start<t_rest)
        t_pos=p_queue->start+p_queue->len;
    else
        t_pos=p_queue->start-t_rest;
    p_queue->buffer[t_pos]=p_data;
    ++p_queue->len;
}

static queue sg_recv;
bool sg_overflow;
bool sg_sending;

static void on_uart_interrupt() interrupt INTERRUPT_NO
{
    if(RI==1)
    {
        if(sg_recv.len==sg_recv.max)
            sg_overflow=1;
        else
            queue_write(&sg_recv,SBUF);
        RI=0;
    }
    else if(TI==1)
    {
        sg_sending=0;
        TI=0;
    }
}

void uart_init(uint8* p_buffer,uint8 p_capacity)
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFA;		//设定定时初值
	TH1 = 0xFA;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
    PS=1;    //串口中断优先级最高
    ES=1;    //打开串口中断
    EA=1;
    sg_recv.buffer=p_buffer;
    sg_recv.max=p_capacity;
    sg_recv.start=0;
    sg_recv.len=0;
    sg_overflow=0;
    sg_sending=0;
}

void uart_close()
{
    TR1=0;
    ES=0;
}

uint8 uart_available()
{
    return sg_recv.len;
}

uint8 uart_read()
{
    uint8 t_data;
    ES=0;
    t_data=queue_read(&sg_recv);
    ES=1;
    return t_data;
}

bool uart_is_overflow()
{
    return sg_overflow;
}

void uart_overflow_reset()
{
    ES=0;
    sg_overflow=0;
    ES=1;
}

void uart_write(uint8 p_data)
{
    while(sg_sending);
    ES=0;
    sg_sending=1;
    SBUF=p_data;
    ES=1;
}

void uart_print(char* p_string)
{
    while(*p_string)
    {
        uart_write(*p_string);
        ++p_string;
    }
}

void uart_println(char* p_string)
{
    uart_print(p_string);
    uart_print("rn");
}