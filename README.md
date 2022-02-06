# NIXIE_Clock-esp32-arduino
使用esp32，8管，自动休眠、氛围灯、休眠和唤醒动画、音乐闹钟、基于Blinker的蓝牙控制  

<p float="left">
  <img src="https://github.com/flashorsink/NIXIE_Clock-esp32-arduino/blob/main/%E6%95%B4%E4%BD%93%E5%9B%BE.jpg" width="600" />
  <img src="https://github.com/flashorsink/NIXIE_Clock-esp32-arduino/blob/main/Blinker.jpg" height="330" /> 
</p>

## 程序

本程序使用esp32 arduino，请使用1.0.6版本，2.0.0及以上版本不兼容无法编译

有两处需要修改部分参见esp32代码libraries文件夹中的说明

各项设置断电后仍会保存

## PCB

全触摸设计，正面盖板没有开孔，电源口居中，辉光管可插拔设计便于移动和更换。

音频芯片TQFN不易焊接，制作部分用了嘉立创SMT，库中有此芯片的装贴。

部分pcb原件以图片形式给出

## 亚克力

亚克力盖板厚度应小于1mm，否则会影响触摸按键效果

## 底座

底座为3D打印，需要一定精度打印M3螺纹，铜柱高度为40mm

## Blinker

使用方式在Blinker文件夹中说明

## 操作
各项设置与闹钟时间断电后仍会保留
触摸包含两个控制触摸按键和一个电源触摸按键
1. 短按：设置分钟
2. 长按：设置小时
3. 同时短按：改变时钟模式，四种模式包括时钟显示、时间线时钟显示和时间线随机显示
4. 同时长按：改变灯光模式，包括单色显示、呼吸、渐变、七彩，切换和唤醒都有过渡动画
5. 当闹铃响起时触摸任意一个控制触摸按键都将关闭闹铃

## 注意安全
辉光管使用170v电压，虽然使用的是12VDC转170VDC电流较小并设计了自恢复保险丝，但仍需要注意安全。

## 感谢
https://github.com/zer0pwned/NixieTubeClock 提供了最初版本的灵感，并使用了其中的辉光插座、时钟芯片和继电器的元件库

https://github.com/Sadudu/MAX1771 提供了12V-170V DC-DC升压电路，经测试输出电容改为电解电容可避免啸叫
