# MyBootloader

## STM32 Blue Pill (STM32F103C8T6) Bootloader

1. 0.95 OLED: PB8,PB9;
2. PB15 和 GND 之间加一个按钮;
3. 通过 USB 串口(Mac 下推荐 Serial 这个 App)使用 Ymodem 协议上传程序(修改 ld 文件 -> FLASH (rx) : ORIGIN = 0x08002000, LENGTH = 128K
   );
4. 刷程序成功后每次自动进入程序,按住 PB15 后 reboot 可以再次进入 menu 刷程序;
