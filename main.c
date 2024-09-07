/* 
 * 本程序实现了一个简单的计步器，通过ADXL345加速度传感器检测人体的步数
 * 通过LCD1602显示步数和距离
 * 通过串口输出步数和距离
 * 通过按键切换显示模式
 * 通过按键复位步数和距离
 * 通过按键测试ADXL345的X,Y,Z轴数据
 * 
 * 作者：北海无河
 * 时间：2024年8月
 * 部分代码以及计步器算法参考文献资料：https://zhou-yuxin.github.io/ 周语馨 from 南京大学 to 英特尔亚太研发有限公司
 *                                  https://www.analog.com/cn/resources/analog-dialogue/articles/pedometer-design-3-axis-digital-acceler.html#author 利用3轴数字加速度计实现功能全面的计步器设计 Neil Zhao 
 * 本项目为浙江大学生物医学工程与仪器科学学院2022级本科二年级暑期电路综合课程设计作业
 * 本项目使用的开发板为STC89C52RC
 * 
 */

#include <reg51.h>
#include "types.h"
#include "i2c.h"
#include "lcd1602.h"
#include "adxl345.h"
#include "buffered_uart.h"
sbit  key1   =P2^5;
sbit  key2   =P2^4;
sbit  key3   =P2^3;
sbit  key4   =P2^2;
sbit  test   =P2^0;

#define ADXL345_ADDR 0x53


void delay_ms(uint16 xms) // xms代表需要延时的毫秒
{
    unsigned int x,y;
    for(x=xms;x>0;x--)
    for(y=110;y>0;y--);
}

int key_function()
{
    delay_ms(2);						 //消抖
    if(key1==0||key2==0||key3==0||key4==0)	 //判断是否被按下
	  {   if(key1==0)
	         return 1;    //返回键值1
	   else if(key2==0)
	         return 2;	  //返回键值2
	   else if(key3==0)
	         return 3;	  //返回键值3
	   else if(key4==0)
	         return 4;	  //返回键值4
	  }
	  return 0;				   				 //没有按下返回0
}


void main()
{
xdata peak_value_t peak ;
xdata axis_info_t cur_sample;
xdata slid_reg_t slid;
xdata uint8 t_buffer[16];
    int32 cnt;
    int32 current_cnt;
    int32 distance;
    int16 show_mode = 0; 
    int16 key_down = 0;
    
    delay_ms(1000);//上电延时 
    uart_init(t_buffer,16);
    //初始化LCD1602
    lcd1602_init();
    lcd1602_display_switch(true,false,false);//显示字符，不显示光标，不闪烁
    lcd1602_clear();
    //显示初始化中
    lcd1602_string_set(0,0,"Initializing...");
    delay_ms(1000);
    ADXL345_Init();
    //检查ADXL345是否正常
    if(i2c_read(ADXL345_ADDR,0x00)!=0xE5){
        lcd1602_string_set(0,1,"ADXL345 error");
        while(1);
    }else{
        lcd1602_string_set(0,1,"ADXL345 OK");
    }
    delay_ms(1000);
    lcd1602_clear();
    //显示初始化完成
    lcd1602_string_set(0,0,"Initialized");
    delay_ms(1000);
    lcd1602_clear();

 //   cnt=114514;
    while(1){
        while(show_mode==0){
            test=1;
            key_down=key_function();
            lcd1602_string_set(0, 0, "Menu");
            lcd1602_string_set(4, 0, "1.Step");
            lcd1602_string_set(10, 0, "2.Dist");
            lcd1602_string_set(0, 1, "3.Test");
            lcd1602_string_set(7, 1, "4.Reset");
            if(key_down==0){
                show_mode=0;
            }else{
                show_mode=key_down;
                lcd1602_clear();
            }
        }
        while(show_mode==1){
            test=1;
            key_down=key_function();
            cnt=detect_step(&peak,&slid,&cur_sample,current_cnt);  //检测步数
            current_cnt=cnt;
            test=0;
            if(key_down==0){
                show_mode=1;
            }else{
                show_mode=key_down;
                lcd1602_clear();
            }
      }
        while(show_mode==2){
            test=1;
            key_down=key_function();
            current_cnt=cnt;
            test=0;
            lcd1602_string_set(0, 0, "Distance:");
            //fix
            if(key_down==0){
                show_mode=2;
            }else{
                show_mode=key_down;
                lcd1602_clear();
            }
        }
        while(show_mode==3){
            key_down=key_function();
            display_x_y_z();
            if(key_down==0){
                show_mode=3;
            }else{
                show_mode=key_down;
                lcd1602_clear();
            }
        }
        while(show_mode==4){
            lcd1602_clear();
            show_mode=0;
            current_cnt=0;
        }
    }
}


