esp32 arduino库与blinker库请自行下载

esp32 arduino包请使用1.0.6版本，2.0.0及以上版本不兼容无法编译

1.0.6版本对ticker.active()函数有定义但没有实现，请手动在ticker.cpp中添加代码，代码已在图“修改1”中标出

代码使用blinker库实现与手机的交互，但由于使用的个数多于官方设定的蓝牙状态下的6个，请手动在BlinkerConfig.h中更改为16，测试表明不会有任何兼容问题，更改处已在“修改2中标出”。

库文件删除了说明等不必要文件，完整文件可访问原网址获得

[ESP32 - Arduino 1.0.6](https://github.com/espressif/arduino-esp32/releases/tag/1.0.6)

[Blinker - v0.3.7](https://github.com/blinker-iot/blinker-library/releases/tag/0.3.7)

[RTCmaster for DS3231](https://github.com/Makuna/Rtc)

[WS2812](https://github.com/Freenove/Freenove_WS2812_Lib_for_ESP32)

[AUDIO use MAX9835）](https://github.com/schreibfaul1/ESP32-audioI2S)

## 感谢各库的原作者！


