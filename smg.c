#include "smg.h"

// 共阴极数码管段码表（0~F，无小数点）
u8 gsmg_code[17] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
    0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
};

// 动态扫描显示函数
// dat: 长度为8的段码数组（若少于8，未指定位置应提前置0）
// pos: 起始显示位置（1~8），从该位开始依次显示dat[0], dat[1]...直到第8位
void smg_display(u8 dat[], u8 pos) {
    u8 i;
    u8 pos_temp = pos - 1;          // 转为数组索引（0开始）
    for(i = pos_temp; i < 8; i++) {
        // 选择第 (7-i) 号数码管（根据硬件连接调整顺序）
        switch(7 - i) {
            case 0: LSC=1; LSB=1; LSA=1; break; // 第1位（最左边）
            case 1: LSC=1; LSB=1; LSA=0; break; // 第2位
            case 2: LSC=1; LSB=0; LSA=1; break;
            case 3: LSC=1; LSB=0; LSA=0; break;
            case 4: LSC=0; LSB=1; LSA=1; break;
            case 5: LSC=0; LSB=1; LSA=0; break;
            case 6: LSC=0; LSB=0; LSA=1; break;
            case 7: LSC=0; LSB=0; LSA=0; break; // 第8位（最右边）
        }
        SMG_A_DP_PORT = dat[i - pos_temp]; // 输出段码
        delay_10us(100);                    // 显示保持时间（约1ms）
        SMG_A_DP_PORT = 0x00;               // 消隐
    }
}