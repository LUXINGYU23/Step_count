#define I2C_SCL P1^0
#define I2C_SDA P1^1
#define I2C_DELAY 10

#include "i2c.h"
#include <reg51.h>

sbit SCL=I2C_SCL;
sbit SDA=I2C_SDA;

static void i2c_delay(uint8 p_loop)
{
    while(p_loop--);
}

static void i2c_start()
{
    SDA=1;
    SCL=1;
    i2c_delay(I2C_DELAY);
    SDA=0;
    i2c_delay(I2C_DELAY);
    SCL=0;
}

static void i2c_stop()
{
    SDA=0;
    SCL=1;
    i2c_delay(I2C_DELAY);
    SDA=1;
    i2c_delay(I2C_DELAY);
}

static void i2c_send_ack(bool p_ack)
{
    SDA=p_ack;
    SCL=1;
    i2c_delay(I2C_DELAY);
    SCL=0;
    i2c_delay(I2C_DELAY);
}

static uint8 i2c_recv_ack()
{
    uint8 t_ack;
    SCL=1;
    i2c_delay(I2C_DELAY);
    t_ack=SDA;
    SCL=0;
    i2c_delay(I2C_DELAY);
    return t_ack;
}

static uint8 i2c_send_byte(uint8 p_data)
{
    uint8 t_i;
    for(t_i=0;t_i<8;t_i++)
    {
        SDA=p_data&0x80;
        p_data<<=1;
        SCL=1;
        i2c_delay(I2C_DELAY);
        SCL=0;
        i2c_delay(I2C_DELAY);
    }
    return i2c_recv_ack();
}

static uint8 i2c_recv_byte()
{
    uint8 t_i;
    uint8 t_data=0;
    SDA=1;
    for(t_i=0;t_i<8;t_i++)
    {
        t_data<<=1;
        SCL=1;
        i2c_delay(I2C_DELAY);
        t_data|=SDA;
        SCL=0;
        i2c_delay(I2C_DELAY);
    }
    return t_data;
}

void i2c_write(uint8 p_address,uint8 p_register,uint8 p_data)
{
    p_address<<=1;
    i2c_start();
    i2c_send_byte(p_address);
    i2c_send_byte(p_register);
    i2c_send_byte(p_data);
    i2c_stop();
}

uint8 i2c_read(uint8 p_address,uint8 p_register)
{
    uint8 t_data;
    i2c_multi_read(p_address,p_register,1,&t_data);
    return t_data;
}

void i2c_multi_write(uint8 p_address,uint8 p_register_start,uint8 p_count,uint8* p_buffer)
{
    uint8 t_i;
    for(t_i=0;t_i<p_count;t_i++)
        i2c_write(p_address,p_register_start+t_i,p_buffer[t_i]);
}

void i2c_multi_read(uint8 p_address,uint8 p_register_start,uint8 p_count,uint8* p_buffer)
{
    uint8 t_i;
    p_address<<=1;
    i2c_start();
    i2c_send_byte(p_address);
    i2c_send_byte(p_register_start);
    i2c_start();
    i2c_send_byte(p_address+1);
    for(t_i=0;t_i<p_count;t_i++)
    {
        p_buffer[t_i]=i2c_recv_byte();
        i2c_send_ack(t_i==p_count-1);
    }
    i2c_stop();
}