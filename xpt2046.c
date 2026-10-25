#include "xpt2046.h"

// 向XPT2046发送8位命令，读取12位ADC结果（返回16位有效值，右对齐）
static u16 XPT2046_Read(u8 cmd) {
    u8 i;
    u16 val = 0;
    XPT_CS = 0;                     // 选中芯片
    // 发送8位命令（高位在前）
    for(i = 0; i < 8; i++) {
        XPT_CLK = 0;
        if(cmd & 0x80) XPT_DIN = 1; else XPT_DIN = 0;
        XPT_CLK = 1;                // 上升沿写入
        cmd <<= 1;
    }
    // 读取12位数据（高位在前）
    for(i = 0; i < 12; i++) {
        XPT_CLK = 0;
        XPT_CLK = 1;               // 下降沿输出数据，上升沿读取
        val <<= 1;
        if(XPT_DOUT) val |= 1;
    }
    XPT_CS = 1;                    // 释放芯片
    return val;
}

void XPT2046_Init(void) {
    XPT_CS = 1;    // 未选中
    XPT_CLK = 0;   // 时钟初始低电平
}

// 读取光敏电阻的ADC值（命令0xA4：单端、通道1、12位、低功耗模式）
u16 XPT2046_ReadLight(void) {
    return XPT2046_Read(0xA4);
}

// 将ADC值转换为光照强度（勒克斯）
// 映射关系：ADC=0 -> 0 Lux; ADC=500 -> 300 Lux; ADC=4095 -> 2000 Lux
u16 ADC_To_Lux(u16 adc_val) {
    if(adc_val <= 500)
        return (u32)adc_val * 300 / 500;          // 0~300 Lux线性
    else
        return 300 + (u32)(adc_val - 500) * (2000 - 300) / (4095 - 500); // 300~2000 Lux线性
}