#ifndef _beep_H
#define _beep_H

#include "public.h"

sbit BEEP = P1^4;   // 蜂鸣器控制引脚（低电平/高电平？实际通过翻转产生脉冲）

void beep_alarm(u16 time, u16 fre); // 蜂鸣报警，time:半周期数，fre:半周期延时（10us单位）

#endif