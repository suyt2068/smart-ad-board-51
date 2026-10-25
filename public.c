#include "public.h"

// 延时 ten_us * 10微秒（通过简单循环消耗时间）
void delay_10us(u16 ten_us) {
    while(ten_us--);
}

// 延时 ms 毫秒（通过两重循环粗略延时，需根据晶振调整内循环次数）
void delay_ms(u16 ms) {
    u16 i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}