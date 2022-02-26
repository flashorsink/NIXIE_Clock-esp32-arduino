## esp32 为 esp32-wroom-32(4MB) esp32-wroom-32D也可以，辉光管为in-14
## TP1是170V升压电路的使能，为了测试其他元件时断开使用的，使用时需焊起来
## BOM添加说明：L3与L4用焊锡连通，C33和C34不焊接。此为预留滤波电路，经过测试可以不使用
## SD卡用作闹钟音乐，可放置多首歌每次响铃会切换，仅支持mp3
## 烧录指南
<p float="left">
  <img src="https://github.com/flashorsink/NIXIE_Clock-esp32-arduino/blob/main/NIXIEClock_version3.2/%E7%83%A7%E5%BD%95.jpg" width="400" />
</p>

1. 通电烧录时使用usb转串口连接RXD、TXD、GND三线并给总电源通电，并打开总电源触摸开关，蓝圈的3.3V不使用

2. 红圈1为esp32的使能，2为esp32的IO0，烧录时先同时按下按键1和按键2，然后先松开按键2再松开按键1，此时esp32进入烧录模式

3. 烧录
 
4. 烧录完成后按按键1并松开，程序运行
