#ifndef _SMG_H
#define _SMG_H

#include "public.h"

#define SMG_A_DP_PORT  P0   // 段码数据端口（P0）

// 位选控制引脚（共阴极数码管，低电平选中）
sbit LSA = P1^0;
sbit LSB = P1^1;
sbit LSC = P1^2;

extern u8 gsmg_code[17];    // 0~F共阴极字形码表

void smg_display(u8 dat[], u8 pos); // 动态扫描显示，dat:8个段码（从第0位开始），pos:起始显示位（1~8）

#endif