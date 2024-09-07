#include "adxl345.h"
void ADXL345_ReadReg_Get_Data(short *x, short *y, short *z)
{
     uint8 BUF[6];
    // 从寄存器0x32开始读取6个字节的数据
    i2c_multi_read(ADXL345_ADDR, 0x32, 6, BUF);
    // 合成数据
    *x = (short)(((uint16)BUF[1] << 8) + BUF[0]);  
    *y = (short)(((uint16)BUF[3] << 8) + BUF[2]);
    *z = (short)(((uint16)BUF[5] << 8) + BUF[4]); 
}
//显示xyz轴
void display_x_y_z(){ 
    float temp;
    short x, y, z;
    uint8 BUF[6];
    // 从寄存器0x32开始读取6个字节的数据
    i2c_multi_read(ADXL345_ADDR, 0x32, 6, BUF);
    // 合成数据
    x = (short)(((int16)BUF[1] << 8) + BUF[0]);  
    y = (short)(((int16)BUF[3] << 8) + BUF[2]);
    z = (short)(((int16)BUF[5] << 8) + BUF[4]); 
	temp = (float) x * 3.9;
	if (temp < 0) {
		lcd1602_char_set(2, 0, '-');
		temp = -temp;
	}
	lcd1602_string_set(0, 0, "X:");
	lcd1602_char_set(3, 0, (int)temp / 1000 + '0');
	lcd1602_char_set(4, 0, '.');
	lcd1602_char_set(5, 0, (int)temp % 1000 / 100 + '0');
	lcd1602_char_set(6, 0, (int)temp % 100 / 10 + '0');
	temp = (float) y * 3.9;
	if (temp < 0) {
		lcd1602_char_set(2, 1, '-');
		temp = -temp;
	}
	lcd1602_string_set(0, 1, "Y:");
	lcd1602_char_set(3, 1, (int)temp / 1000 + '0');
	lcd1602_char_set(4, 1, '.');
	lcd1602_char_set(5, 1, (int)temp % 1000 / 100 + '0');
	lcd1602_char_set(6, 1, (int)temp % 100 / 10 + '0');
	temp = (float) z * 3.9;
	if (temp < 0) {
		lcd1602_char_set(10, 1, '-');
		temp = -temp;
	}
	lcd1602_string_set(8, 1, "Z:");
	lcd1602_char_set(11, 1, (int)temp / 1000 + '0');
	lcd1602_char_set(12, 1, '.');
	lcd1602_char_set(13, 1, (int)temp % 1000 / 100 + '0');
	lcd1602_char_set(14, 1, (int)temp % 100 / 10 + '0');
}
 void ADXL345_Init(void)
{
	i2c_write(ADXL345_ADDR,0x31,0x0B);   //测量范围,正负16g，13位模式
	i2c_write(ADXL345_ADDR,0x2C,0x09);   //速率设定为50Hz 参考pdf13页
	i2c_write(ADXL345_ADDR,0x2D,0x08);   //选择电源模式   参考pdf24页
	i2c_write(ADXL345_ADDR,0x2E,0x80);   //使能 DATA_READY 中断
    i2c_write(ADXL345_ADDR,0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
	i2c_write(ADXL345_ADDR,0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
	i2c_write(ADXL345_ADDR,0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}
 
 

//读取xyz数据存入均值滤波器，存满进行计算，滤波后样本存入sample，计算阈值，构建数据包，发送数据包
void filter_calculate(filter_avg_t *filter, axis_info_t *sample,peak_value_t *peak,int32 ccnt)
{   
    xdata uint8 packet[31]; // 数据包长度为27字节
    xdata uint8 checksum = 0;
	uint8 i;
	short x_sum = 0, y_sum = 0, z_sum = 0;
    short threshold_x, threshold_y, threshold_z;
	for (i = 0; i < FILTER_CNT; i++) {
		ADXL345_ReadReg_Get_Data(&filter->info[i].x, &filter->info[i].y, &filter->info[i].z);
		x_sum += filter->info[i].x;
		y_sum += filter->info[i].y;
		z_sum += filter->info[i].z;
	}
	sample->x = x_sum / FILTER_CNT;
	sample->y = y_sum / FILTER_CNT;
	sample->z = z_sum / FILTER_CNT;
    // 计算阈值
    threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
    threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
    threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
    // 构建数据包
    packet[0] = 0xA5; // 包头
    packet[1] = (uint8)(sample->x & 0xFF); // x低字节
    packet[2] = (uint8)((sample->x >> 8) & 0xFF); // x高字节
    packet[3] = (uint8)(sample->y & 0xFF); // y低字节
    packet[4] = (uint8)((sample->y >> 8) & 0xFF); // y高字节
    packet[5] = (uint8)(sample->z  & 0xFF); // z低字节
    packet[6] = (uint8)((sample->z >> 8) & 0xFF); // z高字节
    // 添加 peak 数据
    packet[7] = (uint8)(peak->oldmax.x & 0xFF); // oldmax.x低字节
    packet[8] = (uint8)((peak->oldmax.x >> 8) & 0xFF); // oldmax.x高字节
    packet[9] = (uint8)(peak->oldmin.x & 0xFF); // oldmin.x低字节
    packet[10] = (uint8)((peak->oldmin.x >> 8) & 0xFF); // oldmin.x高字节
    packet[11] = (uint8)(peak->oldmax.y & 0xFF); // oldmax.y低字节
    packet[12] = (uint8)((peak->oldmax.y >> 8) & 0xFF); // oldmax.y高字节
    packet[13] = (uint8)(peak->oldmin.y & 0xFF); // oldmin.y低字节
    packet[14] = (uint8)((peak->oldmin.y >> 8) & 0xFF); // oldmin.y高字节
    packet[15] = (uint8)(peak->oldmax.z & 0xFF); // oldmax.z低字节
    packet[16] = (uint8)((peak->oldmax.z >> 8) & 0xFF); // oldmax.z高字节
    packet[17] = (uint8)(peak->oldmin.z & 0xFF); // oldmin.z低字节
    packet[18] = (uint8)((peak->oldmin.z >> 8) & 0xFF); // oldmin.z高字节
    // 添加 threshold 数据
    packet[19] = (uint8)(threshold_x & 0xFF); // threshold_x低字节
    packet[20] = (uint8)((threshold_x >> 8) & 0xFF); // threshold_x高字节
    packet[21] = (uint8)(threshold_y & 0xFF); // threshold_y低字节
    packet[22] = (uint8)((threshold_y >> 8) & 0xFF); // threshold_y高字节
    packet[23] = (uint8)(threshold_z & 0xFF); // threshold_z低字节
    packet[24] = (uint8)((threshold_z >> 8) & 0xFF); // threshold_z高字节
    // 添加步数数据
    packet[25] = (uint8)(ccnt & 0xFF); // ccnt低字节
    packet[26] = (uint8)((ccnt >> 8) & 0xFF); // ccnt次低字节
    packet[27] = (uint8)((ccnt >> 16) & 0xFF); // ccnt次高字节
    packet[28] = (uint8)((ccnt >> 24) & 0xFF); // ccnt高字节
    // 计算校验位
    for (i = 1; i <= 28; i++) {
        checksum += packet[i];
    }
    packet[29] = checksum; // 校验位
    packet[30] = 0x5A; // 包尾
    
    // 发送数据包
    send_packet(packet, 31);
}
 
 
//50个样本中找出最大最小值作为动态阈值
//找出最大值和最小值
//已修改为核心函数，返回值为当前步数

int32 cnt_update(peak_value_t *peak, axis_info_t *cur_sample, slid_reg_t *slid, int32 current_cnt)
{
    xdata int8 i;
    xdata char res;
    xdata short x_change;
    xdata short y_change;
    xdata short z_change;
    xdata filter_avg_t filter;
    xdata short min_value = -32768;
    xdata short max_value = 32767;
    xdata short ccnt = 0;
    xdata int interval = 0; // 新增变量记录步伐之间的数据更新次数
    res = MOST_ACTIVE_NULL;
    ccnt = current_cnt;

    peak->newmax.x = min_value;
    peak->newmax.y = min_value;
    peak->newmax.z = min_value;

    peak->newmin.x = max_value;
    peak->newmin.y = max_value;
    peak->newmin.z = max_value;

    for (i = 0; i < SAMPLE_SIZE; i++) {
        filter_calculate(&filter, cur_sample, peak, ccnt);
        cur_sample->x = cur_sample->x;
        cur_sample->y = cur_sample->y;
        cur_sample->z = cur_sample->z;
        peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
        peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
        peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);
        peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
        peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
        peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);

        if (ABS((cur_sample->x - slid->new_sample.x)) > DYNAMIC_PRECISION) {
            slid->old_sample.x = slid->new_sample.x;
            slid->new_sample.x = cur_sample->x;
        } else {
            slid->old_sample.x = slid->new_sample.x;
        }
        if (ABS((cur_sample->y - slid->new_sample.y)) > DYNAMIC_PRECISION) {
            slid->old_sample.y = slid->new_sample.y;
            slid->new_sample.y = cur_sample->y;
        } else {
            slid->old_sample.y = slid->new_sample.y;
        }

        if (ABS((cur_sample->z - slid->new_sample.z)) > DYNAMIC_PRECISION) {
            slid->old_sample.z = slid->new_sample.z;
            slid->new_sample.z = cur_sample->z;
        } else {
            slid->old_sample.z = slid->new_sample.z;
        }

        x_change = ABS((peak->newmax.x - peak->newmin.x));
        y_change = ABS((peak->newmax.y - peak->newmin.y));
        z_change = ABS((peak->newmax.z - peak->newmin.z));

        if (x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION) {
            res = MOST_ACTIVE_X;
        } else if (y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION) {
            res = MOST_ACTIVE_Y;
        } else if (z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION) {
            res = MOST_ACTIVE_Z;
        }

        switch (res) {
            case MOST_ACTIVE_NULL:
                break;
            case MOST_ACTIVE_X: {
                short threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
                if (slid->old_sample.x > threshold_x && slid->new_sample.x < threshold_x) {
                    if (interval >= 3 && interval <= 100) { // 检查时间间隔是否在有效窗口内
                        ccnt++;
                        lcd_step_display(ccnt);
                    }
                    interval = 0; // 重置间隔计数器
                }
                break;
            }
            case MOST_ACTIVE_Y: {
                short threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
                if (slid->old_sample.y > threshold_y && slid->new_sample.y < threshold_y) {
                    if (interval >= 3 && interval <= 100) { // 检查时间间隔是否在有效窗口内
                        ccnt++;
                        lcd_step_display(ccnt);
                    }
                    interval = 0; // 重置间隔计数器
                }
                break;
            }
            case MOST_ACTIVE_Z: {
                short threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
                if (slid->old_sample.z > threshold_z && slid->new_sample.z < threshold_z) {
                    if (interval >= 3 && interval <= 100) { // 检查时间间隔是否在有效窗口内
                        ccnt++;
                        lcd_step_display(ccnt);
                    }
                    interval = 0; // 重置间隔计数器
                }
                break;
            }
            default:
                break;
        }

        interval++; // 增加间隔计数器
    }

    peak->oldmax.x = peak->newmax.x;
    peak->oldmax.y = peak->newmax.y;
    peak->oldmax.z = peak->newmax.z;
    peak->oldmin.x = peak->newmin.x;
    peak->oldmin.y = peak->newmin.y;
    peak->oldmin.z = peak->newmin.z;
    return ccnt;
}
 

void send_packet(uint8 *packet, uint8 length) {
xdata uint8 i;
    for (i = 0; i < length; i++) {
        uart_write(packet[i]);
    }
}

int8 detect_step(peak_value_t *peak, slid_reg_t *slid, axis_info_t *cur_sample,int32 current_cnt) {
    static xdata int32 cnt = 0; // 步数
    cnt = cnt_update(peak, cur_sample,slid,current_cnt);
    return cnt;
}

void lcd_step_display(int32 cnt){
            lcd1602_string_set(0, 0, "Step:");
            // 计算每一位数字并显示
            lcd1602_char_set(5, 0, (cnt / 10000000000) % 10 + '0');
            lcd1602_char_set(6, 0, (cnt / 1000000000) % 10 + '0');
            lcd1602_char_set(7, 0, (cnt / 100000000) % 10 + '0');
            lcd1602_char_set(8, 0, (cnt / 10000000) % 10 + '0');
            lcd1602_char_set(9, 0, (cnt / 1000000) % 10 + '0');
            lcd1602_char_set(10, 0, (cnt / 100000) % 10 + '0');
            lcd1602_char_set(11, 0, (cnt / 10000) % 10 + '0');
            lcd1602_char_set(12, 0, (cnt / 1000) % 10 + '0');
            lcd1602_char_set(13, 0, (cnt / 100) % 10 + '0');
            lcd1602_char_set(14, 0, (cnt / 10) % 10 + '0');
            lcd1602_char_set(15, 0, cnt % 10 + '0');
}




