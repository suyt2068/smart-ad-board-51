#ifndef _WIFI_CONTROL_H
#define _WIFI_CONTROL_H

#include "public.h"

// 平台下发命令定义（单字节）
#define AUTO_MODE           0x01   // 切换为自动模式
#define MANUAL_MODE         0x02   // 切换为手动模式
#define MANUAL_LED_ON       0x03   // 手动模式下开启跑马灯
#define MANUAL_LED_OFF      0x04   // 手动模式下关闭跑马灯
#define MANUAL_SMG_ON       0x05   // 手动模式下开启数码管显示
#define MANUAL_SMG_OFF      0x06   // 手动模式下关闭数码管显示

void wifi_control_init(void);   // 初始化：串口、光敏、ESP8266连接等
void wifi_control(void);        // 主循环：读取光照、更新设备状态、上报数据

#endif