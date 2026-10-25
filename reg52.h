/*--------------------------------------------------------------------------
REG52.H

Header file for generic 80C52 and 80C32 microcontroller.
Copyright (c) 1988-2002 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#ifndef __REG52_H__
#define __REG52_H__

/*  BYTE Registers  */
sfr P0    = 0x80;    // 端口0
sfr P1    = 0x90;    // 端口1
sfr P2    = 0xA0;    // 端口2
sfr P3    = 0xB0;    // 端口3
sfr PSW   = 0xD0;    // 程序状态字
sfr ACC   = 0xE0;    // 累加器
sfr B     = 0xF0;    // B寄存器
sfr SP    = 0x81;    // 堆栈指针
sfr DPL   = 0x82;    // 数据指针低字节
sfr DPH   = 0x83;    // 数据指针高字节
sfr PCON  = 0x87;    // 电源控制寄存器
sfr TCON  = 0x88;    // 定时器控制寄存器
sfr TMOD  = 0x89;    // 定时器模式寄存器
sfr TL0   = 0x8A;    // 定时器0低字节
sfr TL1   = 0x8B;    // 定时器1低字节
sfr TH0   = 0x8C;    // 定时器0高字节
sfr TH1   = 0x8D;    // 定时器1高字节
sfr IE    = 0xA8;    // 中断使能寄存器
sfr IP    = 0xB8;    // 中断优先级寄存器
sfr SCON  = 0x98;    // 串口控制寄存器
sfr SBUF  = 0x99;    // 串口数据缓冲寄存器

/*  8052 Extensions  */
sfr T2CON  = 0xC8;   // 定时器2控制
sfr RCAP2L = 0xCA;   // 定时器2捕获/重载低字节
sfr RCAP2H = 0xCB;   // 定时器2捕获/重载高字节
sfr TL2    = 0xCC;
sfr TH2    = 0xCD;

/*  BIT Registers  */
/*  PSW  */
sbit CY    = PSW^7;
sbit AC    = PSW^6;
sbit F0    = PSW^5;
sbit RS1   = PSW^4;
sbit RS0   = PSW^3;
sbit OV    = PSW^2;
sbit P     = PSW^0; //8052 only

/*  TCON  */
sbit TF1   = TCON^7;
sbit TR1   = TCON^6;
sbit TF0   = TCON^5;
sbit TR0   = TCON^4;
sbit IE1   = TCON^3;
sbit IT1   = TCON^2;
sbit IE0   = TCON^1;
sbit IT0   = TCON^0;

/*  IE  */
sbit EA    = IE^7;   // 全局中断使能
sbit ET2   = IE^5;   // 定时器2中断使能（8052）
sbit ES    = IE^4;   // 串口中断使能
sbit ET1   = IE^3;   // 定时器1中断使能
sbit EX1   = IE^2;   // 外部中断1使能
sbit ET0   = IE^1;   // 定时器0中断使能
sbit EX0   = IE^0;   // 外部中断0使能

/*  IP  */
sbit PT2   = IP^5;
sbit PS    = IP^4;
sbit PT1   = IP^3;
sbit PX1   = IP^2;
sbit PT0   = IP^1;
sbit PX0   = IP^0;

/*  P3  */
sbit RD    = P3^7;
sbit WR    = P3^6;
sbit T1    = P3^5;
sbit T0    = P3^4;
sbit INT1  = P3^3;
sbit INT0  = P3^2;
sbit TXD   = P3^1;
sbit RXD   = P3^0;

/*  SCON  */
sbit SM0   = SCON^7;
sbit SM1   = SCON^6;
sbit SM2   = SCON^5;
sbit REN   = SCON^4;
sbit TB8   = SCON^3;
sbit RB8   = SCON^2;
sbit TI    = SCON^1; // 发送中断标志
sbit RI    = SCON^0; // 接收中断标志

/*  P1  */
sbit T2EX  = P1^1; // 8052 only
sbit T2    = P1^0; // 8052 only

/*  T2CON  */
sbit TF2    = T2CON^7;
sbit EXF2   = T2CON^6;
sbit RCLK   = T2CON^5;
sbit TCLK   = T2CON^4;
sbit EXEN2  = T2CON^3;
sbit TR2    = T2CON^2;
sbit C_T2   = T2CON^1;
sbit CP_RL2 = T2CON^0;

#endif