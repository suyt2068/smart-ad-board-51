#include "uart.h"

#define RELOAD_COUNT 0xFA   // 定时器1重载值，配合PCON=0x80得到9600波特率（11.0592MHz晶振）

void UART_Init(void) {
    SCON = 0x50;      // 模式1（8位UART），允许接收
    TMOD &= 0x0F;     // 清除定时器1模式位
    TMOD |= 0x20;     // 定时器1，模式2（8位自动重载）
    PCON = 0x80;      // SMOD=1，波特率加倍
    TH1 = RELOAD_COUNT; // 设置重载值
    TL1 = TH1;          // 初始值
    TR1 = 1;            // 启动定时器1
    ES = 1;             // 使能串口中断
    EA = 1;             // 总中断使能
}

void UART_SendData(u8 dat) {
    ES = 0;       // 发送时暂时关闭串口中断，避免干扰
    TI = 0;       // 清除发送完成标志
    SBUF = dat;   // 将数据写入发送缓冲，启动发送
    while(!TI);   // 等待发送完成（TI硬件置1）
    TI = 0;       // 清除标志
    ES = 1;       // 恢复串口中断
}

void UART_SendString(u8 *pbuf) {
    while(*pbuf != '\0') {
        UART_SendData(*pbuf++);
        delay_10us(5);  // 短延时，确保不丢数据（可省略）
    }
}