#ifndef _XPT2046_H
#define _XPT2046_H

#include "public.h"

// XPT2046 控制引脚定义（SPI总线）
sbit XPT_CS  = P3^6;   // 片选，低有效
sbit XPT_CLK = P3^7;   // 时钟
sbit XPT_DIN = P3^5;   // 主机输出从机输入（MOSI）
sbit XPT_DOUT = P3^4;  // 主机输入从机输出（MISO）

void XPT2046_Init(void);       // 初始化：CS释放，CLK低电平
u16 XPT2046_ReadLight(void);   // 读取光敏电阻通道（通道1，单端模式）的ADC值
u16 ADC_To_Lux(u16 adc_val);   // 将ADC值（0~4095）转换为光照度（单位：勒克斯，0~2000）

#endif