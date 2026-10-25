# 智能广告牌（居室智能灯光系统）—— 51单片机

基于 51 单片机（AT89C52）的智能广告牌 / 居室智能灯光系统。
通过 ESP8266 连接物联云平台，实现远程控制 LED 跑马灯、数码管显示，并利用触摸屏（XPT2046）采集光照度自动调节。

## 硬件组成

- 主控：51 单片机 AT89C52（Keil C51 工程）
- WiFi 模块：ESP8266（STA 模式，TCP 连接物联平台）
- 显示：数码管（LED 七段码）
- 光照采集：XPT2046（触摸屏 / ADC 接口）
- 输出：LED 跑马灯、蜂鸣器

## 功能

- 自动模式：根据环境光照度自动开关 / 切换 LED 跑马灯（光照不足点亮、充足熄灭）
- 手动模式：通过物联平台远程控制 LED 开关与数码管显示
- 光照度上报：每 5 秒通过 ESP8266 上报光照值到物联平台
- 串口调试：启动时输出 "Smart Light System Ready"

## 文件结构

- main.c 主程序：初始化 WiFi 控制、主循环
- wifi_control.c / wifi_control.h WiFi 控制核心（ESP8266 AT 指令、远程命令解析、光照上报）
- smg.c / smg.h 数码管显示
- xpt2046.c / xpt2046.h 触摸屏 / 光照采集
- uart.c / uart.h 串口通信
- beep.c / beep.h 蜂鸣器
- public.c / public.h 公共头文件与延时函数
- reg52.h 8051 寄存器定义
- Project.uvproj Keil C51 工程文件

## 使用说明

用 Keil C51 打开 Project.uvproj，编译后通过 STC-ISP 等工具烧录到 AT89C52 即可。

## 配置说明

使用前请在 wifi_control.c 中替换以下占位符：

- YOUR_WIFI_SSID / YOUR_WIFI_PASSWORD：你的 WiFi 名称与密码
- YOUR_IOT_SERVER：你的物联平台服务器地址
- YOUR_REGISTER_PACKET：你的物联平台设备注册包
