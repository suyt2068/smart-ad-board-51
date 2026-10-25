#include "public.h"      // 公共数据类型和延时函数
#include "uart.h"        // 串口发送字符串功能
#include "wifi_control.h"// WiFi控制主逻辑

void main() {
    wifi_control_init();                     // 初始化WiFi控制模块（串口、光敏、ESP8266连接等）
    UART_SendString("Smart Light System Ready\r\n"); // 通过串口发送启动提示（可用于调试）
    while(1) {
        wifi_control();                      // 进入主循环：读取光照、更新LED/数码管、上传数据
    }
}