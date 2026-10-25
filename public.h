#ifndef _public_H
#define _public_H

#include <reg52.h>       // 包含8051寄存器定义（P0~P3、SCON、TMOD、中断等）

typedef unsigned int u16;   // 无符号16位整型
typedef unsigned char u8;    // 无符号8位整型
typedef unsigned long u32;   // 无符号32位整型

void delay_10us(u16 ten_us); // 延时 ten_us * 10微秒
void delay_ms(u16 ms);       // 延时毫秒

#endif