#ifndef _UART_H
#define _UART_H

#include "public.h"

void UART_Init(void);          // 初始化串口（模式1，波特率9600，双倍速）
void UART_SendData(u8 dat);    // 发送单个字节
void UART_SendString(u8 *pbuf); // 发送字符串

#endif