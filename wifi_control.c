#include "wifi_control.h"
#include "xpt2046.h"
#include "uart.h"
#include "smg.h"
#include "beep.h"

#define LED_PORT P2                // LED跑马灯连接端口（共阳极，低电平点亮）

#define REGISTER_PACKET  "YOUR_REGISTER_PACKET"  // 云平台注册包（需与平台一致）

// 全局状态变量
bit mode;                    // 0:自动模式, 1:手动模式
bit manual_marquee_enabled;  // 手动模式下跑马灯使能 (1:流动, 0:熄灭)
bit manual_disp_state;       // 手动模式下数码管显示允许 (1:显示, 0:全灭)

u16 current_lux = 0;         // 最新光照值（勒克斯）
u16 display_lux = 0;         // 自动模式下用于显示的稳定光照值（过阈值后锁定）
bit last_threshold_above = 0;// 上次是否超过光照阈值（100 Lux）

u8 led_pattern = 0x01;       // 跑马灯当前流动模式（低电平有效的位）
u16 led_timer = 0;           // LED更新计数器（每10ms+1，达到10时更新一次）
u16 send_timer = 0;          // 数据上报计数器（每10ms+1，达到500即5秒上报一次）

#define LIGHT_THRESHOLD  100  // 光照阈值（单位：勒克斯）

static u8 code hello_seg[5] = {0x76, 0x79, 0x38, 0x38, 0x3f}; // "HELLO"段码（自定义）

// 函数声明
static void update_marquee(void);
static void ESP8266_SendCmd(u8 *pbuf);
static void ESP8266_SendData(u8 *pbuf);
static void ESP8266_StaInit(void);
static void Display_Light(u16 lux);
static void Display_Hello(void);
static void Display_Off(void);
static void process_remote_cmd(u8 cmd);
static void check_and_record_lux(void);

// 更新LED跑马灯状态（根据模式、光照、手动标志）
static void update_marquee(void) {
    if(mode == 0) { // 自动模式
        if(current_lux < LIGHT_THRESHOLD) {   // 光照不足，启动跑马灯
            LED_PORT = ~led_pattern;          // 输出低有效模式
            led_pattern = (led_pattern << 1) | (led_pattern >> 7); // 循环左移
            if(led_pattern == 0x00) led_pattern = 0x01;
        } else {
            LED_PORT = 0xFF;                  // 光照充足，全部熄灭
        }
    } else { // 手动模式
        if(manual_marquee_enabled) {
            LED_PORT = ~led_pattern;
            led_pattern = (led_pattern << 1) | (led_pattern >> 7);
            if(led_pattern == 0x00) led_pattern = 0x01;
        } else {
            LED_PORT = 0xFF;                  // 手动关闭跑马灯
        }
    }
}

// 发送AT指令（自动添加\r\n）
static void ESP8266_SendCmd(u8 *pbuf) {
    while(*pbuf) {
        UART_SendData(*pbuf++);
        delay_10us(5);
    }
    UART_SendData('\r');
    UART_SendData('\n');
    delay_ms(1000);   // 等待命令执行
}

// 通过ESP8266发送数据（先发送AT+CIPSEND=长度，再发数据）
static void ESP8266_SendData(u8 *pbuf) {
    u8 len = 0;
    u8 *p = pbuf;
    while(*p++) len++;                     // 计算数据长度
    // 发送 "AT+CIPSEND=xx"
    UART_SendString("AT+CIPSEND=");
    UART_SendData(len/10 + '0');
    UART_SendData(len%10 + '0');
    UART_SendData('\r');
    UART_SendData('\n');
    delay_ms(100);                         // 等待模块准备接收
    while(*pbuf) {
        UART_SendData(*pbuf++);
    }
    UART_SendData('\n');                   // 发送结束符（部分固件要求）
    delay_ms(100);
}

// ESP8266初始化：设置为STA模式，连接Wi-Fi，连接TCP服务器，发送注册包
static void ESP8266_StaInit(void) {
    ESP8266_SendCmd("AT+RST");          // 复位模块
    delay_ms(3000);
    ESP8266_SendCmd("AT+CWMODE=1");     // 设置为STA模式
    ESP8266_SendCmd("AT+CIPMUX=0");     // 单连接模式
    delay_ms(500);
    // 连接Wi-Fi（请根据实际修改SSID和密码）
    ESP8266_SendCmd("AT+CWJAP=\"YOUR_WIFI_SSID\",\"YOUR_WIFI_PASSWORD\"");
    delay_ms(8000);
    ESP8266_SendCmd("AT+CIFSR");        // 查询IP（可选）
    delay_ms(1000);
    // 连接TCP服务器（域名和端口）
    ESP8266_SendCmd("AT+CIPSTART=\"TCP\",\"YOUR_IOT_SERVER\",28801");
    delay_ms(3000);
    // 发送注册包，告知云平台设备身份
    ESP8266_SendData(REGISTER_PACKET);
    delay_ms(500);
}

// 数码管显示光照数值（自动格式：数字 + " L" 末尾显示 "HE" 单位）
static void Display_Light(u16 lux) {
    u8 buf[8];
    u8 i, len = 0;
    u8 digits[4];

    for(i = 0; i < 8; i++) buf[i] = 0x00;

    // 分解lux的十进制位
    if(lux >= 1000) {
        digits[0] = lux / 1000;
        digits[1] = (lux % 1000) / 100;
        digits[2] = (lux % 100) / 10;
        digits[3] = lux % 10;
        len = 4;
    } else if(lux >= 100) {
        digits[0] = lux / 100;
        digits[1] = (lux % 100) / 10;
        digits[2] = lux % 10;
        len = 3;
    } else if(lux >= 10) {
        digits[0] = lux / 10;
        digits[1] = lux % 10;
        len = 2;
    } else {
        digits[0] = lux;
        len = 1;
    }

    // 填入数字段码
    for(i = 0; i < len; i++)
        buf[i] = gsmg_code[digits[i]];

    // 后两位显示 "HE"（单位勒克斯的示意）
    buf[len] = 0x00;        // 熄灭
    buf[len+1] = 0x38;      // "L"（实际是 "L" 段码？根据实际字形）
    buf[len+2] = 0x76;      // "H"（字符H）
    // 这里显示 "数值 + LH" 表示光照

    smg_display(buf, 1);
}

// 显示 "HELLO"（开机或光照充足时自动模式显示）
static void Display_Hello(void) {
    u8 buf[8];
    u8 i;
    for(i = 0; i < 8; i++) buf[i] = 0x00;
    for(i = 0; i < 5; i++) buf[i] = hello_seg[i];
    smg_display(buf, 1);
}

// 关闭所有数码管显示
static void Display_Off(void) {
    u8 buf[8] = {0x00};
    smg_display(buf, 1);
}

// 处理云平台下发的单字节命令
static void process_remote_cmd(u8 cmd) {
    switch(cmd) {
        case AUTO_MODE:
            mode = 0;
            led_pattern = 0x01;
            LED_PORT = ~led_pattern;
            break;
        case MANUAL_MODE:
            mode = 1;
            update_marquee();
            break;
        case MANUAL_LED_ON:
            if(mode == 1) {
                manual_marquee_enabled = 1;
                led_pattern = 0x01;
                update_marquee();
            }
            break;
        case MANUAL_LED_OFF:
            if(mode == 1) {
                manual_marquee_enabled = 0;
                LED_PORT = 0xFF;
            }
            break;
        case MANUAL_SMG_ON:
            if(mode == 1) manual_disp_state = 1;
            break;
        case MANUAL_SMG_OFF:
            if(mode == 1) manual_disp_state = 0;
            break;
        default:
            break;
    }
}

// 检查光照阈值变化，触发蜂鸣器报警，并在自动模式下记录显示值
static void check_and_record_lux(void) {
    static bit last = 0;
    static bit first_call = 1;
    bit current = (current_lux >= LIGHT_THRESHOLD);
    
    if(first_call) {
        first_call = 0;
        last = current;          // 上电时不触发蜂鸣
    } else if(current != last) {
        last = current;
        beep_alarm(100, 10);    // 阈值变化时短鸣一声
        if(mode == 0 && current) {   // 自动模式下从暗→亮，记录当前光照值用于显示
            display_lux = current_lux;
        }
    }
    last_threshold_above = current;
}

// WiFi控制模块初始化
void wifi_control_init(void) {
    UART_Init();                // 串口初始化（用于ESP8266通信）
    XPT2046_Init();             // 光敏ADC初始化
    ESP8266_StaInit();          // 连接WiFi和云平台

    mode = 0;                   // 默认自动模式
    manual_marquee_enabled = 0;
    manual_disp_state = 1;

    LED_PORT = 0xFF;            // LED全部熄灭
    BEEP = 0;                   // 蜂鸣器静音
    ES = 1;                     // 确保串口中断使能（已在UART_Init中开启，这里冗余）
    EA = 1;

    last_threshold_above = 0;
    display_lux = 0;
}

// 主循环函数（应在while(1)中持续调用）
void wifi_control(void) {
    u16 adc_val;
    u8 lux_str[9];
    u16 report_lux;

    while(1) {
        // 读取当前光照值
        adc_val = XPT2046_ReadLight();
        current_lux = ADC_To_Lux(adc_val);

        // 检查阈值变化（并触发蜂鸣）
        check_and_record_lux();

        // 每100ms更新一次LED跑马灯（因为外层delay_ms(10)，led_timer加到10即为100ms）
        if(++led_timer >= 10) {
            led_timer = 0;
            update_marquee();
        }

        // 每5秒上报一次光照数据（500 * 10ms = 5000ms）
        if(++send_timer >= 500) {
            send_timer = 0;
            if(mode == 0 && last_threshold_above) {
                display_lux = current_lux;   // 自动模式下更新显示用光照值
            }
            report_lux = current_lux;
            // 构造 "LUX:xxxx" 字符串
            lux_str[0] = 'L'; lux_str[1] = 'U'; lux_str[2] = 'X'; lux_str[3] = ':';
            lux_str[4] = (report_lux / 1000) % 10 + '0';
            lux_str[5] = (report_lux / 100) % 10 + '0';
            lux_str[6] = (report_lux / 10) % 10 + '0';
            lux_str[7] = report_lux % 10 + '0';
            lux_str[8] = '\0';
            ESP8266_SendData(lux_str);       // 上报给云平台
        }

        // 数码管显示逻辑
        if(mode == 0) {
            if(last_threshold_above)
                Display_Light(display_lux);   // 自动模式光照充足显示记录的光照值
            else
                Display_Hello();              // 自动模式光照不足显示"HELLO"
        } else {
            if(manual_disp_state)
                Display_Light(current_lux);   // 手动模式显示实时光照
            else
                Display_Off();                // 手动模式关闭显示
        }

        delay_ms(10);   // 主循环周期10ms
    }
}

// 串口中断服务函数（用于解析ESP8266接收到的 "+IPD,<len>:<data>"）
void UART_IRQn() interrupt 4 {
    static u8 step = 0;          // 解析状态机状态
    static u8 data_len = 0;      // 数据长度
    static u8 recv_cnt = 0;      // 已接收字节计数
    u8 ch;

    if(RI) {
        RI = 0;
        ch = SBUF;

        // 解析 "+IPD,<len>:<data>" 格式，提取最后一个字节作为命令
        switch(step) {
            case 0:
                if(ch == '+') { step = 1; data_len = 0; recv_cnt = 0; }
                break;
            case 1:
                if(ch == 'I') step = 2; else step = 0;
                break;
            case 2:
                if(ch == 'P') step = 3; else step = 0;
                break;
            case 3:
                if(ch == 'D') step = 4; else step = 0;
                break;
            case 4:
                if(ch == ',') step = 5; else step = 0;
                break;
            case 5:
                if(ch >= '0' && ch <= '9')
                    data_len = data_len * 10 + (ch - '0');
                else if(ch == ':') step = 6;
                else step = 0;
                break;
            case 6:
                recv_cnt++;
                if(recv_cnt == data_len) {
                    process_remote_cmd(ch);   // 假设命令是数据段的最后一个字节
                    step = 0;
                    data_len = 0;
                }
                break;
            default:
                step = 0;
                break;
        }
    }
}