//#define TEST
#ifdef TEST
    #define SLEEPTIME 50//5s
#else
#define SLEEPTIME 50  //5s
#endif
#include<string.h>
#include<stdio.h>
char str[50];
char str2[4];
/////////////////// data storage configuration/////////////////////////////
#include <Preferences.h>
Preferences STORGE; 
///////////////////DS3231 configuration///////////////////////
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
#define RtcSquareWavePin 5 //alarm pin
#define countof(a) (sizeof(a) / sizeof(a[0]))
RtcDS3231<TwoWire> Rtc(Wire);//SDA21 SCL22
volatile bool interruptFlag = false;//alarm interrupt
RtcDateTime oldtime;
RtcDateTime now;
bool flashnow=true;
///////////////////Scan configuration/////////////////////
#define SCAN  34//18
//////////////////Ticker clock configuration///////////////////////////
#include <Ticker.h>
Ticker tickerclock;
Ticker tickerchange;
volatile bool timetoflash = false;
bool clockflag=false;
volatile int clock_counter=0;
bool scanhuman=true;
///////////////////RGB configuration///////////////////////////
volatile int8_t gradient=0;//0 means nothing;1 means changemode-1 light down;2 means changemode light up;3 means changemode light down;
#include "Freenove_WS2812_Lib_for_ESP32.h"
#define LEDS_COUNT  8 //how many led 
#define LEDS_PIN  12
#define CHANNEL   0
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
//bool rgb_switch=false;
bool rgb_sleep=false;
uint8_t color_r=0,color_g=0,color_b=0,bright=0;
int8_t RGBmode=1;
bool RGBflag=false;
Ticker tickerRGB;
//////////////////Touch configuration//////////////////////
Ticker tickertouch;
int8_t gottouch1=0;
int8_t gottouch2=0;
#define TOUCHPAD_1 T3//gpio13
#define TOUCHPAD_2 T4//gpio15

int8_t level_1; // 
int8_t level_2; // 

#define TOUCH1 1
#define TOUCH2 2
#define BOTHTOUCH_SHORT 3
#define BOTHTOUCH_LONG 4
#define TOUCH1_LONG 5
#define TOUCH2_LONG 6
bool touch1=false;
bool touch2=false;
bool touchdone=false;//indicate if finger has moed away 
int  touch=0; //1:button1 2:botton2 3:botton 1and2 short 4:botton 1 and 2 long
///////////////////music configuration///////////////////////
#include "Audio.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#define SD_CS         4
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26
Audio audio;
char songname[30][50];
int songnumber=0;
volatile bool songflag=false;
bool songon=false;
//////////////////////74hc595 confifuration////////////////////
unsigned char LED_0F[] = 
    {
        B10000000,B00000000,//0
    	B00000000,B01000000,//1
        B00000000,B10000000,//2
        B00000001,B00000000,//3
        B00000010,B00000000,//4
        B00000100,B00000000,//5 
        B00001000,B00000000,//6
        B00010000,B00000000,//7
        B00100000,B00000000,//8
        B01000000,B00000000,//9
        B00000000,B00100000,//.
        B00000000,B00000000,//none
        B10000000,B00000000,//0
    	B00000000,B01000000,//1
        B00000000,B10000000,//2
        B00000001,B00000000,//3
        B00000010,B00000000,//4
        B00000100,B00000000,//5 
        B00001000,B00000000,//6
        B00010000,B00000000,//7
        B00100000,B00000000,//8
        B01000000,B00100000,//9+.
    };
#define NONE 11
#define DOT 10
    unsigned char LED[8]={11,11,11,11,11,11,11,11};//cache for 8 figures initial none
    int SCLK = 14;
    int RCLK = 17;
    int DIO = 16; 
int8_t clockmode=1;
//////////////////////ALARM ////////////////////////////////
   bool alarmswitch,alarmsingle;
   bool day[7];
   int8_t alarmh,alarmm;
//////////////////////blinker configuration/////////////////////

#define BLINKER_BLE  
#define BLINKER_PRINT Serial
#include "Blinker.h"
int32_t voice=5;
bool switchset=0;
BlinkerRGB RGB("RGB");
//BlinkerButton RGBS_SWITCH("cs");
BlinkerButton RGBS_SLEEP("ls");
BlinkerButton HOUR_UP("hu");
BlinkerButton HOUR_DOWN("hd");
BlinkerButton MINUTE_UP("mu");
BlinkerButton MINUTE_DOWN("md");
BlinkerButton SECOND_UP("su");
BlinkerButton SECOND_DOWN("sd");
BlinkerButton SWITCH("sw");
BlinkerButton SCANHUMAN("sc");
BlinkerButton CLOCKMODE("cm");
BlinkerText TEXT1("t1");
BlinkerText TEXT2("t2");
BlinkerText TEXT3("t3");
BlinkerText DATE("d");
BlinkerSlider VOICE("v");
BlinkerButton RGBMODE("rm");
BlinkerSlider RGBSPEED("rs");
BlinkerButton MON("m");
BlinkerButton TUE("t");
BlinkerButton WED("w");
BlinkerButton THU("T");
BlinkerButton FRI("f");
BlinkerButton SAT("s");
BlinkerButton SUN("S");
BlinkerButton ALARMSWITCH("as");
BlinkerButton ALARMSINGLE("ss");
BlinkerText ALARM("al");
BlinkerSlider ALARMH("ah");
BlinkerSlider ALARMM("am");

int8_t rgbspeed=15;
bool touchbegin=false;


void setup () 
{
/////////////////////HC595///////////////////////////////////////
    pinMode(SCLK,OUTPUT);
    pinMode(RCLK,OUTPUT);
    pinMode(DIO,OUTPUT);
//    digitalWrite(RCLK,LOW);
//    digitalWrite(RCLK,HIGH);
    for(int i=0;i<=7;i++) LED[i]=NONE;LED_Display();
 ///////////////////////Serial Setup/////////////////////////////    
#ifdef TEST   
    Serial.begin(115200); 
#endif
////////////////////////////////////////////////////////////////////
    randomSeed(analogRead(35));
/////////////////////////Touchpad Setup/////////////////////////////
    touchpadsetup();
///////////////////storage setup////////////////////////////
    STORGE.begin("mynamespace");
    scanhuman=STORGE.getBool("scanhuman",true);
//   rgb_switch=STORGE.getBool("rgb_switch",false);
    rgb_sleep=STORGE.getBool("rgb_sleep",false);
    color_r=STORGE.getUChar("color_r",255);
    color_g=STORGE.getUChar("color_g",255);
    color_b=STORGE.getUChar("color_b",255);
    bright=STORGE.getUChar("bright",50);
    RGBmode=STORGE.getUChar("RGBmode",1);
    voice=STORGE.getUInt("voice",5);
    rgbspeed=STORGE.getUChar("rgbspeed",15);
    clockmode=STORGE.getUChar("clockmode",1);
    if(clockmode==4) clockmode=1;
    day[1]=STORGE.getBool("day[1]",false);
    day[2]=STORGE.getBool("day[2]",false);
    day[3]=STORGE.getBool("day[3]",false);
    day[4]=STORGE.getBool("day[4]",false);
    day[5]=STORGE.getBool("day[5]",false);
    day[6]=STORGE.getBool("day[6]",false);
    day[0]=STORGE.getBool("day[0]",false);
    alarmswitch=STORGE.getBool("alarmswitch",false);
    alarmh=STORGE.getUChar("alarmh",10);
    alarmm=STORGE.getUChar("alarmm",30);
    alarmsingle=STORGE.getBool("alarmsingle",false);

///////////////////////DS3231 setup/////////////////////////////
    Rtc.Begin();
    RtcDateTime now ;
    now = Rtc.GetDateTime();
    oldtime=now;
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); //how many alarm you want, up to 2
/////////////////////RGB setup////////////////////////////////////
    strip.begin();
/////////////////////RGB test/////////////////////////////////////
    strip.setBrightness(0);  
    strip.setAllLedsColorData(0,0,0);
    strip.show();
    if (RGBmode!=5) tickerRGB.attach_ms(110-rgbspeed,RGBchange);
#ifdef TEST
    Serial.print("110-rgbspeed:");
    Serial.println(110-rgbspeed);
#endif 
////////////////////humanscan setup////////////////////////////////    
    pinMode(SCAN, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(SCAN), InterruptServiceRoutine_scanhuman, RISING);
    if(scanhuman)tickerclock.attach_ms(100,clocksever,SLEEPTIME); 
   
///////////////////music setup/////////////////////////////////
    pinMode(SD_CS, OUTPUT);      
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    SPI.setFrequency(10000000);
    SD.begin(SD_CS);
    File dir=SD.open("/");
     while (true)
    {
 
        File entry = dir.openNextFile();
        if (!entry)
        {
            // 如果没有文件则跳出循环
            break;
        }
        if(strstr(entry.name(), ".mp3")!=NULL)
        {
            strcpy(songname[songnumber],entry.name());
            songnumber++;
        }
        entry.close();
    }
    dir.close();
    #ifdef TEST
        Serial.println("MUSIC LIST:");
        for(int i=0;i<songnumber;i++)// show music list 
        {
            Serial.println(songname[i]);
        }
    #endif
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(voice); // 0...21   
 
/////////////////start sceen//////////////////////////////////
    startshow();

///////////////////blinker/////////////////////
    Blinker.begin();
    Blinker.attachHeartbeat(heartbeat); 
    RGB.attach(rgb_callback);
 // RGBS_SWITCH.attach(rgbswitch_callback);
    RGBS_SLEEP.attach(rgbsleep_callback);
    RGBMODE.attach(rgbmode_callback);
    HOUR_UP.attach(hourup_callback);
    HOUR_DOWN.attach(hourdown_callback);
    MINUTE_UP.attach(minuteup_callback);
    MINUTE_DOWN.attach(minutedown_callback);
    SECOND_UP.attach(secondup_callback);
    SECOND_DOWN.attach(seconddown_callback);
    SWITCH.attach(switch_callback);
    SCANHUMAN.attach(scanhuman_callback);
    VOICE.attach(voice_callback);
    RGBSPEED.attach(rgbspeed_callback);
    CLOCKMODE.attach(clockmode_callback);
    MON.attach(mon_callback);
    TUE.attach(tue_callback);
    WED.attach(wed_callback);
    THU.attach(thu_callback);
    FRI.attach(fri_callback);
    SAT.attach(sat_callback);
    SUN.attach(sun_callback);
    ALARMH.attach(alarmh_callback);
    ALARMM.attach(alarmm_callback);
    ALARMSWITCH.attach(alarmswitch_callback);
    ALARMSINGLE.attach(alarmsingle_callback);
///////////////////////////////////////////////////
 touch=0;
    touch1=false;
    touch2=false;
    touchbegin=false;
    tickertouch.attach_ms(100,Touchsever);
}


void loop () 
{  
    Blinker.run();
    if(flashnow)
        {
           
            now = Rtc.GetDateTime();flashnow=false;
        }
    if(clockflag){clockchange();clockflag=false;}
    if(RGBflag){RGBsever(false);RGBflag=false;}
    Clockshow();
    Blinker.run();
    Alarmed();
    Touchpocess();  
    Storge();
}

void touchpadsetup()
{
    int8_t TOUCHLEVEL_1[9]; 
    int8_t TOUCHLEVEL_2[9]; 

    for(int8_t i=8;i>=0;i--)//get 9 touchvalue
    {
        TOUCHLEVEL_1[i]=touchRead(TOUCHPAD_1);
        TOUCHLEVEL_2[i]=touchRead(TOUCHPAD_2);
    }

    int8_t temp;
    for(int8_t i=8;i>=1;i--)//sort
    {
        for(int8_t j=0;j<=i-1;j++)
        {
            if(TOUCHLEVEL_1[j]>=TOUCHLEVEL_1[j+1]){temp=TOUCHLEVEL_1[j+1];TOUCHLEVEL_1[j+1]=TOUCHLEVEL_1[j];TOUCHLEVEL_1[j]=temp;}
            if(TOUCHLEVEL_2[j]>=TOUCHLEVEL_2[j+1]){temp=TOUCHLEVEL_2[j+1];TOUCHLEVEL_2[j+1]=TOUCHLEVEL_2[j];TOUCHLEVEL_2[j]=temp;}
        }
    }
    level_1=TOUCHLEVEL_1[4];//use the median value
    level_2=TOUCHLEVEL_2[4];


#ifdef TEST
    Serial.print("TOUCHLEVEL1=");  for(int8_t i=8;i>=0;i--) {Serial.print(TOUCHLEVEL_1[i]);} Serial.println("");
    Serial.print("TOUCHLEVEL2=");  for(int8_t i=8;i>=0;i--) {Serial.print(TOUCHLEVEL_2[i]);} Serial.println("");
    Serial.print("level1=");        Serial.println(level_1);
    Serial.print("level2=");        Serial.println(level_2);
#endif
touchAttachInterrupt(TOUCHPAD_1, GotTouch_1, level_1-6);
touchAttachInterrupt(TOUCHPAD_2, GotTouch_2, level_2-6);
}

bool changevoice=false,changergb=false,changealarmh=false,changealarmm=false,changergb2=false;
void Storge()
{
    static int32_t i=0;
    if(changevoice||changergb||changealarmh||changealarmm){i++;}
    if(i>700000)
    {
        i=0;
        if(changevoice)
        {
                {STORGE.putUInt("voice",voice);changevoice=false;
                #ifdef TEST 
                    Serial.println("change1!");
                #endif
                }
        }
        if(changergb)
        {
                {STORGE.putUChar("rgbspeed",rgbspeed);changergb=false;
                #ifdef TEST 
                    Serial.println("change2!");
                #endif
                }
        }
        if(changealarmh||changealarmm)
        {
            showalarm();
            if(changealarmh)
                {STORGE.putUChar("alarmh",alarmh);changealarmh=false;
                #ifdef TEST 
                    Serial.println("change3!");
                #endif
                }
            if(changealarmm)
                {STORGE.putUChar("alarmm",alarmm);changealarmm=false;
                #ifdef TEST 
                    Serial.println("change4!");
                #endif
                } 
        }     
        if(changergb2)
        {
            STORGE.putUChar("color_r",color_r);
            STORGE.putUChar("color_g",color_g);
            STORGE.putUChar("color_b",color_b);
            STORGE.putUChar("bright",bright);
            #ifdef TEST 
            Serial.println("changergb!");
            #endif
        }   
    }    
}
bool clockgradient=false;bool clockgradienton=false;
void InterruptServiceRoutine_scanhuman()
{   
    #ifdef TEST
    Serial.println("trigger!!");
    #endif
   // Clockshow(); overflow!may beacause of i2c
    clock_counter=0;
    
    if(scanhuman && clockmode!=3) 
    {
        timetoflash=true;   
        if(!tickerclock.active()) 
        {
             clockgradient=true; 
             clockgradienton=true; 
             tickerclock.attach_ms(100,clocksever,SLEEPTIME);
             if(!tickerchange.active()){ tickerchange.attach_ms(50,clock2);}
        }
    }
}

volatile bool closeflag=false;
int8_t count10=9;

void Clockshow()
{
    if(closeflag && !tickerchange.active()) 
    {
        timetoflash=false;
        Serial.println("close0");
        closeflag=false;
        clockgradient=true;
        clockgradienton=false;
        if(!tickerchange.active()) tickerchange.attach_ms(50,clock2);
        // for(int i=0;i<=7;i++) 
        //     LED[i]=NONE;LED_Display();
        if(rgb_sleep)
        {  
            gradient=3;
            if(!tickerRGB.active()) tickerRGB.attach_ms(110-rgbspeed,RGBchange);

            // strip.setBrightness(0);  
            // strip.setAllLedsColorData(0,0,0);
            // strip.show();
            // if(tickerRGB.active())tickerRGB.detach();
        }
    }
    
        if(timetoflash==true||!scanhuman||clockmode==3||songon)
        {
            if(rgb_sleep && !tickerRGB.active()){gradient=2; tickerRGB.attach_ms(110-rgbspeed,RGBchange);}
            timetoflash=false;
            if(oldtime!=now)
                {
                count10++;
                if (count10>10) count10=10;
                oldtime=now;
                printDateTime();
                }
        }
    
    
}
void clocksever(int counter)
{
        timetoflash=true;
        if(scanhuman&&clockmode!=3&&clockmode!=4)
        {
            bool scanhumanflag;
            scanhumanflag=digitalRead(SCAN);
            if (scanhumanflag==true)
            {            
                clock_counter=0;
            }    
            else {
                clock_counter++;    
                if(clock_counter>=counter)//100*100ms=10s
                {
                    clock_counter=0;
                    closeflag=true;
                    // if(!tickerchange.active()) tickerchange.attach_ms(50,clock2);
                    if(tickerclock.active())tickerclock.detach();
                    return;
                }    
            }
        }
}

void printDateTime()
{
    #ifdef TEST
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            now.Month(),
            now.Day(),
            now.Year(),
            now.Hour(),
            now.Minute(),
            now.Second() );
    Serial.println(datestring);
    #endif
    tickerchange.attach_ms(50,clock2);
}
void clock2()
{
    clockflag=true;
}

void clockchange()
{   
   
    if(clockgradient)
    {
        static int8_t gradientnumber=0; 
        bool mask[7];
            if(clockmode==1) {mask[7]=true;mask[6]=true;mask[5]=false;mask[4]=true;mask[3]=true;mask[2]=false;mask[1]=true;mask[0]=true;}
            if(clockmode==2) {mask[7]=true;mask[6]=true;mask[5]=true;mask[4]=true;mask[3]=true;mask[2]=true;mask[1]=true;mask[0]=true;}
            if(clockmode==3) {mask[7]=true;mask[6]=true;mask[5]=true;mask[4]=true;mask[3]=true;mask[2]=true;mask[1]=true;mask[0]=true;}
            if(clockmode==4) {mask[7]=false;mask[6]=false;mask[5]=false;mask[4]=false;mask[3]=false;mask[2]=false;mask[1]=false;mask[0]=false;}
            for (int i=0;i<=7;i++)
            {
                if (mask[i]==true) LED[i]=random(10);
            }
            LED_Display();
            gradientnumber++;
            if(gradientnumber>=5)
            {
                gradientnumber=0;
                clockgradient=false;
                if(clockgradienton==false)
                {
                    clockgradienton=true;
                    if(tickerchange.active())tickerchange.detach();
                    for(int i=0;i<=7;i++) 
                    LED[i]=NONE;LED_Display();
                    closeflag=false;
                    return;
                }
            }
            
    }
    else
    {
        static int8_t t=0;
        int8_t x;
        int8_t h1=(now.Hour()/10);int8_t h2=(now.Hour()%10);int8_t m1=(now.Minute()/10); int8_t m2=(now.Minute()%10); int8_t s1=(now.Second()/10); int8_t s2=(now.Second()%10);
        
        if(clockmode==1)
        {
            t++;
            if(s1==0 && s2==0 && t<=8) 
            {LED[7]=random(7)+3+12;LED[6]=random(10)+12;LED[5]=NONE;LED[4]=random(3)+7+12;LED[3]=random(10)+12;LED[1]=random(3)+7+12;LED[0]=random(10)+12;LED[2]=NONE;LED_Display();}
            else if(s2==0 && t<=4) 
            {LED[7]=h1;LED[6]=h2;LED[5]=NONE;LED[4]=m1;LED[3]=m2;LED[1]=random(3)+7;LED[0]=s2;LED[2]=NONE;LED_Display();}
            else 
                {          
                    t=0;  
                    LED[7]=h1;LED[6]=h2;LED[5]=NONE;LED[4]=m1;LED[3]=m2;LED[1]=s1;LED[0]=s2;LED[2]=NONE;LED_Display();
                    if(tickerchange.active())tickerchange.detach();
                }
        }
        if(clockmode==2)
        {
            static int8_t y=0;
            t++;
            if(s2==0 && t<=8) 
            {   x=random(9)+2;if(x==10)x=0;
                LED[7]=x+12;
                LED[6]=random(10);LED[5]=random(7)+3+12;LED[4]=random(10)+12;LED[3]=random(3)+7+12;LED[2]=random(10)+12;LED[1]=random(3)+7+12;LED[0]=random(10)+12;LED_Display();
                if(t==8)
                {if(random(10)==9) y=0;else y=1; }
            }
            else
            
                {          
                    t=0;
                    LED[7]=y;LED[6]=DOT;LED[5]=h1;LED[4]=h2;LED[3]=m1;LED[2]=m2;LED[1]=s1;LED[0]=s2;LED_Display();
                    if(tickerchange.active())tickerchange.detach();
                }
        }
        if(clockmode==3)
        {
        if (count10>=10)
            {   
                t++;
                if(t<=8) { x=random(9)+2;if(x==10)x=0;LED[7]=x;LED[6]=random(10)+12;LED[5]=random(7)+3+12;LED[4]=random(10)+12;LED[3]=random(3)+7+12;LED[2]=random(10)+12;LED[1]=random(3)+7+12;LED[0]=random(10)+12;LED_Display();}
                    else 
                    {   
                    count10=0;      
                    t=0;  
                    if(random(10)==9) x=0;else x=1;  
                    LED[7]=x;LED[6]=DOT;LED[5]=random(10);LED[4]=random(10);LED[3]=random(10);LED[2]=random(10);LED[1]=random(10);LED[0]=random(10);LED_Display();
                    if(tickerchange.active())tickerchange.detach();
                    }
            }
            else{ if(tickerchange.active())tickerchange.detach();}
        }
        if(clockmode==4)
        {
            for (int i=0;i<=7;i++) LED[i]=NONE;
            LED_Display();
            if(tickerchange.active())tickerchange.detach();
        }
    }    
}

void startshow()
{   
    for(int i=0;i<=170;i++)
    {
        for (int j=0;j<=7;j++) LED[j]=random(10);
        LED_Display();
        RGBsever(true);
        delay(18);
    }
    now = Rtc.GetDateTime();
    int8_t h1=(now.Hour()/10);int8_t h2=(now.Hour()%10);int8_t m1=(now.Minute()/10); int8_t m2=(now.Minute()%10); int8_t s1=(now.Second()/10); int8_t s2=(now.Second()%10);
    RGBsever(false); 
    timetoflash=true;    
    clockchange();  
    timetoflash=true; 
}

void LED_OUT(unsigned char LED1,unsigned char LED2)
{
    	unsigned char i;
    	for(i=8;i>=1;i--)
    	{
    		if (LED1&0x80) 
                {
                  digitalWrite(DIO,HIGH);
                 }  
                else 
                {
                  digitalWrite(DIO,LOW);
                }
    		LED1<<=1;
                digitalWrite(SCLK,LOW);
                digitalWrite(SCLK,HIGH);
    	}
        for(i=3;i>=1;i--)
    	{
    		if (LED2&0x80) 
                {
                  digitalWrite(DIO,HIGH);
                 }  
                else 
                {
                  digitalWrite(DIO,LOW);
                }
    		LED2<<=1;
                digitalWrite(SCLK,LOW);
                digitalWrite(SCLK,HIGH);
    	}
}

void LED_Display ()
{
    	unsigned char *led_table;          // 查表指针

    	for(int i=0;i<=7;i++)
        {
            led_table = LED_0F + 2*LED[i];           
    	    LED_OUT(*led_table,*(led_table+1));	
        }	
        digitalWrite(RCLK,LOW);
        digitalWrite(RCLK,HIGH);
}

void RGBchange()
{
    RGBflag=true;
}

#define GRADIENTINTERVAL 10
void RGBsever(bool starts)
{
    
    static int gradientbright=0;
    static bool RGBreverse=1;//reverse flag
    static int x;
    // Serial.print(gradient);
    // Serial.print("  ");Serial.print(RGBmode);Serial.print("  ");Serial.println(gradientbright);
    int8_t a;
    static int8_t j; 
    static int8_t c; 
    if(RGBmode==1 && gradient==1) gradient=2;
    if(RGBmode==5 && gradient==2) gradient=0;
    if(RGBmode==5 && gradient==3) gradient=0;
    if (!starts)
    {
        if (RGBmode==1)
        {
            if(gradient==3)
            {
                gradientbright+=GRADIENTINTERVAL;
                if (bright-gradientbright<0){gradientbright=bright;}
                strip.setBrightness(bright-gradientbright);
                strip.setAllLedsColorData(color_r,color_g,color_b); 
                strip.show();
                if((bright-gradientbright)==0){gradient=2;gradientbright=0; if(tickerRGB.active())tickerRGB.detach(); return;}
            }
            else
            if(gradient==2)
            {
                gradientbright+=GRADIENTINTERVAL;
                if (gradientbright >= bright){gradient=0;gradientbright=0;return;}
                strip.setBrightness(gradientbright);
                strip.setAllLedsColorData(color_r,color_g,color_b); 
                strip.show();  
            }
            else
            {
                strip.setBrightness(bright);
                strip.setAllLedsColorData(color_r,color_g,color_b); 
                strip.show();  
                //if(tickerRGB.active())tickerRGB.detach();  
            }
        }
        else if(RGBmode==2)
        {
            if(gradient==3)
            {
                a=bright/45;
                if(a==0) a=1;
                RGBreverse=0;
                if(RGBreverse==1) x+=a; else x-=a;
                if (x>bright){x=bright;RGBreverse=0;} else if(x<1){x=0;RGBreverse=1;}
                strip.setBrightness(x);
                strip.setAllLedsColorData(color_r,color_g,color_b);
                strip.show();
                if(x==0){gradient=2;gradientbright=0;if(tickerRGB.active())tickerRGB.detach();return;}
            }
            else
            if(gradient==1)
            {
                gradientbright+=GRADIENTINTERVAL;
                if (bright-gradientbright<0){gradient=2;gradientbright=0;return;}
                strip.setBrightness(bright-gradientbright);
                strip.setAllLedsColorData(color_r,color_g,color_b); 
                strip.show();
            }
            else
            {
                if(gradient==2){RGBreverse=1;x=0;gradient=0;}
                a=bright/45;
                if(a==0) a=1;
                if(RGBreverse==1) x+=a; else x-=a;
                if (x>bright){x=bright;RGBreverse=0;} else if(x<1){x=1;RGBreverse=1;}
                strip.setBrightness(x);
                strip.setAllLedsColorData(color_r,color_g,color_b);
                strip.show();
            }
        }
        else
        if(RGBmode==3)
        {
            if(gradient==3)
            {
                gradientbright+=GRADIENTINTERVAL;
                if (bright-gradientbright<0){gradientbright=bright;}
                strip.setBrightness(bright-gradientbright);
                j++;
                strip.setAllLedsColorData(strip.Wheel(j));
                strip.show();
                if((bright-gradientbright)==0){gradient=2;gradientbright=0; if(tickerRGB.active())tickerRGB.detach();return;}
            }
            else
            if(gradient==1)
            {
                a=bright/45;
                if(a==0) a=1;
                RGBreverse=0;
                if(RGBreverse==1) x+=a; else x-=a;
                if (x>bright){x=bright;RGBreverse=0;} else if(x<1){x=0;RGBreverse=1;}
                strip.setBrightness(x);
                strip.setAllLedsColorData(color_r,color_g,color_b);
                strip.show();
                 if(x==0){gradient=2;gradientbright=0;return;}
            }
            else 
            {
                if(gradient==2)
                {
                    gradientbright+=GRADIENTINTERVAL;
                     if(gradientbright>=bright) gradientbright=bright;
                    strip.setBrightness(gradientbright);
                }
                else strip.setBrightness(bright);
                if(gradientbright==bright && gradient!=0){gradientbright=0;gradient=0;}
                j++;
                strip.setAllLedsColorData(strip.Wheel(j));
                strip.show();
            }
        }
        else if(RGBmode==4)    
        {
            if(gradient==3)
            {
                gradientbright+=GRADIENTINTERVAL;
                if (bright-gradientbright<0){gradientbright=bright;}
                strip.setBrightness(bright-gradientbright);
                c++;
                for (int b = 0; b < LEDS_COUNT; b++) 
                    strip.setLedColorData(b, strip.Wheel((b * 256 / LEDS_COUNT + c) & 255));
                strip.show();
                if((bright-gradientbright)==0){gradient=2;gradientbright=0; if(tickerRGB.active())tickerRGB.detach();return;}
            }
            else
            if(gradient==1)
            {
                gradientbright+=GRADIENTINTERVAL;
                if (bright-gradientbright<0){gradient=2;gradientbright=0;return;}
                strip.setBrightness(bright-gradientbright);
                j++;
                strip.setAllLedsColorData(strip.Wheel(j));
                strip.show();
            }
            else
            {
                if(gradient==2)
                {
                    gradientbright+=GRADIENTINTERVAL;
                        if(gradientbright>=bright) {gradientbright=0;gradient=0;return;}
                    strip.setBrightness(gradientbright);
                }
                else strip.setBrightness(bright);
                c++;
                for (int b = 0; b < LEDS_COUNT; b++) 
                    strip.setLedColorData(b, strip.Wheel((b * 256 / LEDS_COUNT + c) & 255));
                strip.show();
            }
        }
        else if(RGBmode==5)
        {
            if(gradient==1)
            {
                gradientbright+=GRADIENTINTERVAL;
                if (bright-gradientbright<0){gradient=0;gradientbright=0;return;}
                strip.setBrightness(bright-gradientbright);
                c++;
                for (int b = 0; b < LEDS_COUNT; b++) 
                    strip.setLedColorData(b, strip.Wheel((b * 256 / LEDS_COUNT + c) & 255));
                strip.show();
            }   
            else
            {
                strip.setBrightness(0);  
                strip.setAllLedsColorData(0,0,0);
                strip.show();
                if(tickerRGB.active())tickerRGB.detach();
            }
        }
    }
    if(starts) 
       {
            static bool RGBflag2=1;//reverse flag
            static int x2;
            int8_t a2;
            a2=255/40;
            if(a2==0) a2=1;
            if(RGBflag2==1) x2+=a2; else x2-=a2;
            if (x2>255){x2=255;RGBflag2=0;} else if(x2<1){x2=1;RGBflag2=1;}
            strip.setBrightness(x2);
            strip.setAllLedsColorData(255,255,255);
            strip.show();
            if(RGBmode!=5) gradient=2;else gradient=0;
       }
}

////alarm process
bool alarmonce=false;
bool touchfreshonce=false;
int nowsongnumber=0;
void Alarmed()
{ 
    //everyday 3:00 fresh touchpadsetup
    if(touchfreshonce==true && now.Minute()!=0) touchfreshonce=false; 
    if(now.Hour()==3 && now.Minute()==0 && touchfreshonce==false) {touchpadsetup();touchfreshonce=true;}
    //alarm
    if(alarmonce==true && now.Minute()!=alarmm) alarmonce=false;
    if(alarmswitch==true)
    {
        if(now.Hour()==alarmh && now.Minute()==alarmm)
        {
            if(alarmsingle==true){
                alarmswitch=false;songflag=true;timetoflash=true;ALARMSWITCH.print("off");STORGE.putBool("alarmswitch",alarmswitch);
                audio.connecttoSD(songname[nowsongnumber]);
               // audio.connecttoSD("/ISeeFire.mp3");
                #ifdef TEST
                    Serial.println("Alarmed single");
                #endif
                
            } 
            else
            if(day[now.DayOfWeek()]==true && alarmonce==false)
            {
                alarmonce=true;songflag=true;timetoflash=true;
               // audio.connecttoSD("/ISeeFire.mp3");
               audio.connecttoSD(songname[nowsongnumber]);
                #ifdef TEST
                    Serial.println("Alarmed nomal");
                #endif
            }
        }
    }
    ////song process
    if(songflag==true)
        {
            songon=true;
            audio.loop();
            //Clockshow();//remember change to shrink clock
        }
    else 
    if(songon==true && songflag==false) //songon shouldn't be deleted.songflag is to show instruct and songon is to show state
    {
       
        audio.stopSong();
        nowsongnumber++;
        if(nowsongnumber>=songnumber){nowsongnumber=0;}
        songon=false;
    }   
}
void audio_eof_mp3(const char *info)
{
    #ifdef TEST
        Serial.print("eof_mp3     ");
    #endif
     audio.connecttoSD(songname[nowsongnumber]);
 //   audio.connecttoSD("/ISeeFire.mp3");
}
////////////////////////////////////////////////////////////////////////////////////             TOUCH            ////////////////////////////////////////////////////////////////////////////////////////////////////
void Touchpocess()
{
    if(touch!=0)
    { 
        RtcDateTime now = Rtc.GetDateTime();
            switch (touch)
            {
            case 1: //minup
                    #ifdef  TEST
                        Serial.println("touchpad 1 trigger");
                    #endif
                    now+=60;
                    Rtc.SetDateTime(now);
                    timetoflash=true;  
                    clock_counter=0;
                    if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
                    break;
            case 2: //mindown
                    #ifdef  TEST
                        Serial.println("touchpad 2 trigger");
                    #endif
                    now-=60;
                    Rtc.SetDateTime(now);
                    timetoflash=true;  
                    clock_counter=0;
                    if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
                    break;
            case 3: //clockmode change
                    #ifdef  TEST
                        Serial.println("touchpadboth_short trigger");
                    #endif
                    clockmode++;
                    if(clockmode>4) {clockmode=1;  if(scanhuman)SCANHUMAN.print("on"); else SCANHUMAN.print("off");   if(rgb_sleep)RGBS_SLEEP.print("on"); else RGBS_SLEEP.print("off");  }
                    if(clockmode==4||clockmode==3) {SCANHUMAN.print("off");RGBS_SLEEP.print("off");}
                    if(clockmode==3) count10=9;
                    clock_counter=0;
                    if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
                    clockgradient=true;
                    clockgradienton=true;
                    if(!tickerchange.active()){tickerchange.attach_ms(50,clock2);}
                    STORGE.putUChar("clockmode",clockmode);
                    break;
            case 4: //RGBmodechange
                    #ifdef  TEST
                        Serial.println("touchpadboth_long trigger");
                    #endif
                    RGBmode++;gradient=1;
                    if(RGBmode>5){RGBmode=1;}
                    if(RGBmode==5){RGBS_SLEEP.print("off");}
                    if(RGBmode==1&&clockmode!=4&&clockmode!=3){if(rgb_sleep)RGBS_SLEEP.print("on");else RGBS_SLEEP.print("off");}
                    if(RGBmode!=5)
                    {
                            if(!tickerclock.active()) {clockgradient=true; clockgradienton=true;tickerclock.attach_ms(100,clocksever,50);if(!tickerchange.active())tickerchange.attach_ms(50,clock2);}
                            if(!tickerRGB.active()){gradient=2;tickerRGB.attach_ms(110-rgbspeed,RGBchange);}
                    } 
                    STORGE.putUChar("RGBmode",RGBmode); 
                    break;
            case 5: //hourup
                    #ifdef  TEST
                        Serial.println("touchpad1_long trigger");
                    #endif
                    now+=3600;
                    Rtc.SetDateTime(now);
                    timetoflash=true;
                    clock_counter=0;
                    if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
                    break;
            case 6: //hourdown
                    #ifdef  TEST
                        Serial.println("touchpad2_long trigger");
                    #endif
                    now-=3600;
                    Rtc.SetDateTime(now);
                    timetoflash=true; 
                    clock_counter=0;
                    if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);} 
                    break;
            default:
                break;
            }
            touch=0;
            
    }
}

void Touchsever()
{   
    //Serial.print(touchRead(TOUCHPAD_1));Serial.print(" ");Serial.println(touchRead(TOUCHPAD_2));
    flashnow=true;
    static int8_t i;
    static bool flag=false; 
        if(!flag&& (gottouch1!=0||gottouch2!=0)) { i++; flag=true;}
        if(gottouch1==0&&gottouch2==0&&flag){flag=false;i=0;}                       
        if(i>3) {i=0;gottouch1=0;gottouch2=0;flag=false;}   
    if(touchbegin==true)
    {
        
        static int touchcounter=0;
        touchcounter++;
        if(touchcounter>=20 && touchcounter<=100)
        {
           // Serial.println("i am in2");
            //touchdone=false;
            if(touch1==true && touch2==true){ touch=BOTHTOUCH_LONG;}
            else if(touch1==true) {touch=TOUCH1_LONG;}
            else {touch= TOUCH2_LONG;}
            touchcounter=101;            
        }
        //scan if finger have moved away
        
            if(touchRead(TOUCHPAD_1)>(level_1-3) && touchRead(TOUCHPAD_2)>(level_2-3)) 
            {
                touchdone=true; touchbegin=false; 
                if(touchcounter<=100)
                {
                    if(touch1==true)//
                    {   
                        if(touch2==true){touch=BOTHTOUCH_SHORT;} else {touch=TOUCH1;}
                        
                    }
                    else if(touch2==true) 
                    {   
                        if(touch1==true){ touch=BOTHTOUCH_SHORT; }else {touch=TOUCH2;}
                        
                    }
                }
                touchcounter=0;
                touch1=false;
                touch2=false;

            }
        
        
    }
}
void GotTouch_1()
{ 
    if(!touch1)
    {
        gottouch1++;
        if(gottouch1>=3) 
        {
            gottouch1=0;
            if(songflag==true)
                {
                    touch=0;
                    songflag=false;
                    touchdone=false;                                                                                                                                                         
                }
            else    
            if (touchbegin==false||touch1==false)
            {
                touchdone=false;
                touch1=true;
                touchbegin=true;
            }
        }
    }
}
void GotTouch_2()
{   
    if(!touch2)
    {
        gottouch2++;
        if(gottouch2>=3) 
        {
            gottouch1=0;
            if(songflag==true)
                {
                touch=0;
                songflag=false;
                touchdone=false;
                }
            else    
            if (touchbegin==false||touch2==false)
            {
                touchdone=false;
                touch2=true;
                touchbegin=true;
       
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////                    BLINKER                    /////////////////////////////////////////////////////////////////////////
void rgb_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
    color_r=r_value;       color_g=g_value;      color_b=b_value;     bright=bright_value;
    if (RGBmode!=5)
    {
        if(!tickerclock.active()) {clockgradient=true; clockgradienton=true;tickerclock.attach_ms(100,clocksever,50);if(!tickerchange.active())tickerchange.attach_ms(50,clock2);}
        if(!tickerRGB.active()){gradient=2;tickerRGB.attach_ms(110-rgbspeed,RGBchange);}
    }
    changergb2=true;
    
}

void rgbsleep_callback(const String & state)
{
    if(scanhuman)
    {
        if(RGBmode!=5&&clockmode!=4&&clockmode!=3)
        {
            if(state=="on")
            {   
                RGBS_SLEEP.print("on");
                rgb_sleep=true;
                clock_counter=0;
                if(!tickerclock.active()){clockgradient=true; clockgradienton=true; tickerclock.attach_ms(100,clocksever,50); if(!tickerchange.active())tickerchange.attach_ms(50,clock2);}
                 if(!tickerRGB.active()) {gradient=2; tickerRGB.attach_ms(110-rgbspeed,RGBchange);}
            }
            else
            if(state=="off")
            {
                RGBS_SLEEP.print("off");
                rgb_sleep=false;
                if(!tickerRGB.active()){gradient=2;tickerRGB.attach_ms(110-rgbspeed,RGBchange);}
            }
            STORGE.putBool("rgb_sleep",rgb_sleep);
        }
    }
}
void rgbmode_callback(const String & state)
{
    RGBmode++;gradient=1;
    if(RGBmode>5){RGBmode=1;}
    if(RGBmode==5){RGBS_SLEEP.print("off");}
    if(RGBmode==1&&clockmode!=4&&clockmode!=3){if(rgb_sleep)RGBS_SLEEP.print("on");else RGBS_SLEEP.print("off");}
    if(RGBmode!=5)
    {
            if(!tickerclock.active()) {clockgradient=true; clockgradienton=true;tickerclock.attach_ms(100,clocksever,50);if(!tickerchange.active())tickerchange.attach_ms(50,clock2);}
            if(!tickerRGB.active()){gradient=2;tickerRGB.attach_ms(110-rgbspeed,RGBchange);}
    } 
    // switch(RGBmode)
    // {
    //     case 1:RGBMODE.text("mode1"); break;
    //     case 2:RGBMODE.text("mode2");break;
    //     case 3:RGBMODE.text("mode3");break;
    //     case 4:RGBMODE.text("mode4");break;
    //     case 5:RGBMODE.text("Close");break;
    // }
    STORGE.putUChar("RGBmode",RGBmode); 
}
void rgbspeed_callback(int32_t value)
{
    rgbspeed=value;
    if(RGBmode==1&&clockmode!=4&&clockmode!=3){if(rgb_sleep)RGBS_SLEEP.print("on");else RGBS_SLEEP.print("off");}
    if(RGBmode!=5)
    {
            if(!tickerclock.active()) {clockgradient=true; clockgradienton=true;tickerclock.attach_ms(100,clocksever,50);if(!tickerchange.active())tickerchange.attach_ms(50,clock2);}
            if(!tickerRGB.active()){gradient=2;tickerRGB.attach_ms(110-rgbspeed,RGBchange);}
    } 
    changergb=true;
}
void showdate(RtcDateTime now)
{
    
    switch (now.DayOfWeek())
        {
            case 0:strcpy(str2, "日");break;
            case 1:strcpy(str2, "一");break;
            case 2:strcpy(str2, "二");break;
            case 3:strcpy(str2, "三");break;
            case 4:strcpy(str2, "四");break;
            case 5:strcpy(str2, "五");break;
            case 6:strcpy(str2, "六");break;
        }
    sprintf(str,"%d年%d月%d日 星期%s",now.Year(),now.Month(),now.Day(),str2);
    DATE.print(str);
}
void showalarm()
{
    
    if(alarmsingle)
    {
        strcpy(str,"单次");
        sprintf(str,"%s %d:%d",str,alarmh,alarmm);
    }
    else
    {
        strcpy(str,"每周");
        if(day[1])strcat(str,"一");
        if(day[2])strcat(str,"二");
        if(day[3])strcat(str,"三");
        if(day[4])strcat(str,"四");
        if(day[5])strcat(str,"五");
        if(day[6])strcat(str,"六");
        if(day[0])strcat(str,"日");
        // if(alarmh<=9 && alarmm<=9)  sprintf(str,"%s 0%d:0%d",str,alarmh,alarmm);
        //  else if(alarmh<=9)    sprintf(str,"%s 0%d:%d",str,alarmh,alarmm);
        //  else if(alarmm<=9)    sprintf(str,"%s %d:0%d",str,alarmh,alarmm);
        sprintf(str,"%s %02d:%02d",str,alarmh,alarmm);
    }
    ALARM.print(str);
}

void hourup_callback(const String & state)
   {
       if(switchset==1)
       {
            RtcDateTime now = Rtc.GetDateTime();
            now=RtcDateTime(now.Year()+1,now.Month(),now.Day(),now.Hour(),now.Minute(),now.Second());
            Rtc.SetDateTime(now);
            timetoflash=true;
           clock_counter=0;
            if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
            showdate(now);
       }
       else
       {
            RtcDateTime now = Rtc.GetDateTime();
            now+=3600;
            Rtc.SetDateTime(now);
            timetoflash=true;
            clock_counter=0;
            if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
    //    now= Rtc.GetDateTime();     
    //    Blinker.print(now.Hour());
       }
       
   }
void hourdown_callback(const String & state)
   {
    if(switchset==1)
       {
            RtcDateTime now = Rtc.GetDateTime();
            now=RtcDateTime(now.Year()-1,now.Month(),now.Day(),now.Hour(),now.Minute(),now.Second());
            Rtc.SetDateTime(now);
            timetoflash=true;
             clock_counter=0;
            if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
            showdate(now);
       }
    else
    {
            RtcDateTime now = Rtc.GetDateTime();
            now-=3600;
            Rtc.SetDateTime(now);
            timetoflash=true; 
            clock_counter=0;
             if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);} 
    //    now= Rtc.GetDateTime();     
    //    Blinker.print(now.Hour());
    }   
      
   }
void minuteup_callback(const String & state)
   {
    if(switchset==1)
    {
        RtcDateTime now = Rtc.GetDateTime();
        int t1,t2;
        if(now.Month()>=12) t1=1; else t1=now.Month()+1;
        if(now.Day()>dayinquire(now.Year(),t1)) t2=dayinquire(now.Year(),t1); else t2=now.Day();
        now=RtcDateTime(now.Year(),t1,t2,now.Hour(),now.Minute(),now.Second());
        Rtc.SetDateTime(now);
        timetoflash=true;
        clock_counter=0;
         if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
        showdate(now);
        
    }
    else
    {
       RtcDateTime now = Rtc.GetDateTime();
       now+=60;
       Rtc.SetDateTime(now);
       timetoflash=true;  
       clock_counter=0;
        if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
    }   
   }
void minutedown_callback(const String & state)
   {
    if(switchset==1)
    {
        int8_t t1,t2;      
        RtcDateTime now = Rtc.GetDateTime();
        if(now.Month()<=1) t1=12; else t1=now.Month()-1;
        if(now.Day()>dayinquire(now.Year(),t1)) t2=dayinquire(now.Year(),t1); else t2=now.Day();
        now=RtcDateTime(now.Year(),t1,t2,now.Hour(),now.Minute(),now.Second());
        Rtc.SetDateTime(now);
        timetoflash=true;
         clock_counter=0;
       if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
        showdate(now);
    }
    else
    {
       RtcDateTime now = Rtc.GetDateTime();
       now-=60;
       Rtc.SetDateTime(now);
       timetoflash=true;
        clock_counter=0;
       if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
    }   
   }
int  dayinquire(int year, int month) 
{
    int days = 0;
    if (month != 2) 
    {
        switch (month) {
            case 1: case 3: case 5: case 7: case 8: case 10: case 12:
                    days = 31;
                    break;
            case 4: case 6: case 9: case 11:
                    days = 30;
                    break;
            }
        } 
    else 
    {// 闰年
        if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
        days = 29;
        else
        days = 28;
    }
    return days;
}
void secondup_callback(const String & state)
   {
    if(switchset==1)
    {
        RtcDateTime now = Rtc.GetDateTime();
        int t;
        if (now.Day()>=dayinquire(now.Year(),now.Month()) ) t=1; else t=now.Day()+1; 
        now=RtcDateTime(now.Year(),now.Month(),t,now.Hour(),now.Minute(),now.Second());
        Rtc.SetDateTime(now);   
        timetoflash=true;
         clock_counter=0;
         if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
        showdate(now);
    }
    else
    {
       RtcDateTime now = Rtc.GetDateTime();
       now+=1;
       Rtc.SetDateTime(now);
       timetoflash=true;
        clock_counter=0;
        if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
    }    
   }
void seconddown_callback(const String & state)
   {
    if(switchset==1)
    {
        RtcDateTime now = Rtc.GetDateTime();
        int t;
        if (now.Day()<=1) t=dayinquire(now.Year(),now.Month()); else t=now.Day()-1; 
        now=RtcDateTime(now.Year(),now.Month(),t,now.Hour(),now.Minute(),now.Second());
        Rtc.SetDateTime(now);
        timetoflash=true;
         clock_counter=0;
         if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
       showdate(now);
    }
    else
    {
       RtcDateTime now = Rtc.GetDateTime();
       now-=1;
       Rtc.SetDateTime(now);
       clock_counter=0;
       if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
    }   
   }
void switch_callback(const String & state)
{
    if(state=="on")
    {
        switchset=1;
        SWITCH.print("on");
        TEXT1.print("Year");
        TEXT2.print("Month");
        TEXT3.print("Day");
    }
    else
    {
        switchset=0;
        SWITCH.print("off");
        TEXT1.print("Hour");
        TEXT2.print("Minute");
        TEXT3.print("Second");
    }
}

void voice_callback(int32_t value)
{
    voice=value;
    audio.setVolume(voice);
    changevoice=true;
    
}
void scanhuman_callback(const String & state)
{
    if(clockmode!=4&&clockmode!=3)
    {
        if(state=="on")
        {
            scanhuman=true;
            clock_counter=0;
            if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);
            SCANHUMAN.print("on");
            if(RGBmode!=5)
            {
                if(rgb_sleep)RGBS_SLEEP.print("on"); else RGBS_SLEEP.print("off");  
            }
        }
        else
        {
            scanhuman=false;
            SCANHUMAN.print("off");
            RGBS_SLEEP.print("off");
        }
        STORGE.putBool("scanhuman",scanhuman);
    }
}

void clockmode_callback(const String & state)
{
    clockmode++;
    if(clockmode>4) {clockmode=1;  if(scanhuman)SCANHUMAN.print("on"); else SCANHUMAN.print("off");   if(rgb_sleep)RGBS_SLEEP.print("on"); else RGBS_SLEEP.print("off");  }
    if(clockmode==4||clockmode==3) {SCANHUMAN.print("off");RGBS_SLEEP.print("off");}
    if(clockmode==3) count10=9;
    clock_counter=0;
    if(scanhuman){if(tickerclock.active())tickerclock.detach();tickerclock.attach_ms(100,clocksever,100);}
    clockgradient=true;
    clockgradienton=true;
    if(!tickerchange.active()){tickerchange.attach_ms(50,clock2);}
    // switch(clockmode)
    // {
    //     case 1:CLOCKMODE.text("mode1"); break;
    //     case 2:CLOCKMODE.text("mode2");break;
    //     case 3:CLOCKMODE.text("mode3");break;
    //     case 4:CLOCKMODE.text("Close");break;
    // }
    STORGE.putUChar("clockmode",clockmode);
}

void mon_callback(const String & state)
{
    if(!alarmsingle){
        day[1]=!day[1];
        STORGE.putBool("day[1]",day[1]);
        if(day[1]){MON.print("on");}else{MON.print("off");}
        showalarm();
    }
    
}
void tue_callback(const String & state)
{
    if(!alarmsingle)
    {
        day[2]=!day[2];
        STORGE.putBool("day[2]",day[2]);
        if(day[2]){TUE.print("on");}else{TUE.print("off");}
        showalarm();
    }
}
void wed_callback(const String & state)
{
    if(!alarmsingle)
    {
        day[3]=!day[3];
        STORGE.putBool("day[3]",day[3]);
        if(day[3]){WED.print("on");}else{WED.print("off");}
        showalarm();
    }
}
void thu_callback(const String & state)
{
    if(!alarmsingle)
    {
    day[4] =!day[4];
    STORGE.putBool("day[4]",day[4]);
    if(day[4]){THU.print("on");}else{THU.print("off");}
    showalarm();
    }
}
void fri_callback(const String & state)
{
    if(!alarmsingle)
    {
        day[5]=!day[5];
        STORGE.putBool("day[5]",day[5]);
        if(day[5]){FRI.print("on");}else{FRI.print("off");}
        showalarm();
    }    
}
void sat_callback(const String & state)
{
    if(!alarmsingle)
    {
        day[6]=!day[6];
        STORGE.putBool("day[6]",day[6]);
        if(day[6]){SAT.print("on");}else{SAT.print("off");}
        showalarm();
    }
}
void sun_callback(const String & state)
{
    if(!alarmsingle)
    {
        day[0]=!day[0];
        STORGE.putBool("day[0]",day[0]);
        if(day[0]){SUN.print("on");}else{SUN.print("off");}
        showalarm();
    }
}
void alarmh_callback(int32_t value)
{
    alarmh=value;
    changealarmh=true;  
}
void alarmm_callback(int32_t value)
{
    alarmm=value;
    changealarmm=true;
}
void alarmswitch_callback(const String & state)
{
    
     if(state=="on")
    {
        alarmswitch=true;
         ALARMSWITCH.print("on");
    }
    else
    {
        alarmswitch=false;
        ALARMSWITCH.print("off");
    }
    STORGE.putBool("alarmswitch",alarmswitch);
    showalarm();
}
void alarmsingle_callback(const String & state)
{
    if(state=="on")
    {
        alarmsingle=true;
        ALARMSINGLE.print("on");
       FRI.print("off");MON.print("off");TUE.print("off");WED.print("off");THU.print("off");SAT.print("off");SUN.print("off");
    }
    else
    {
        alarmsingle=false;
        ALARMSINGLE.print("off");
        if(day[1]){MON.print("on");}else{MON.print("off");}
        if(day[2]){TUE.print("on");}else{TUE.print("off");}
        if(day[3]){WED.print("on");}else{WED.print("off");}
        if(day[4]){THU.print("on");}else{THU.print("off");}
        if(day[5]){FRI.print("on");}else{FRI.print("off");}
        if(day[6]){SAT.print("on");}else{SAT.print("off");}
        if(day[0]){SUN.print("on");}else{SUN.print("off");}
    }
    STORGE.putBool("alarmsingle",alarmsingle);
    showalarm();
}
void heartbeat()
{
    RGB.print(color_r, color_g, color_b, bright);
    if(switchset==0)
    {
        SWITCH.print("off");
        TEXT1.print("Hour");TEXT2.print("Minute");TEXT3.print("Second");
    }
    else
    {
        SWITCH.print("on");
        TEXT1.print("Year");TEXT2.print("Month");TEXT3.print("Day");
    }

    if (clockmode==4||clockmode==3)
    {
        RGBS_SLEEP.print("off");SCANHUMAN.print("off");
    }
    else if(RGBmode==5)
    {
        if(scanhuman){
            SCANHUMAN.print("on");}
        else SCANHUMAN.print("off");
        RGBS_SLEEP.print("off");
    }
    else
    {
        if(scanhuman)
        {
            if(rgb_sleep)RGBS_SLEEP.print("on");else RGBS_SLEEP.print("off");
            SCANHUMAN.print("on");}
        else {SCANHUMAN.print("off");RGBS_SLEEP.print("off");}
       
    }

    // switch(RGBmode)
    // {
    //     case 1:RGBMODE.text("mode1"); break;
    //     case 2:RGBMODE.text("mode2");break;
    //     case 3:RGBMODE.text("mode3");break;
    //     case 4:RGBMODE.text("mode4");break;
    //     case 5:RGBMODE.text("Close");break;
    // }
    //   switch(clockmode)
    // {
    //     case 1:CLOCKMODE.text("mode1"); break;
    //     case 2:CLOCKMODE.text("mode2");break;
    //     case 3:CLOCKMODE.text("mode3");break;
    //     case 4:CLOCKMODE.text("Close");break;
    // }

    if(alarmsingle==true){ALARMSINGLE.print("on");FRI.print("off");MON.print("off");TUE.print("off");WED.print("off");THU.print("off");SAT.print("off");SUN.print("off");}
    else
    {
        ALARMSINGLE.print("off");
        if(day[1]){MON.print("on");}else{MON.print("off");}
        if(day[2]){TUE.print("on");}else{TUE.print("off");}
        if(day[3]){WED.print("on");}else{WED.print("off");}
        if(day[4]){THU.print("on");}else{THU.print("off");}
        if(day[5]){FRI.print("on");}else{FRI.print("off");}
        if(day[6]){SAT.print("on");}else{SAT.print("off");}
        if(day[0]){SUN.print("on");}else{SUN.print("off");}
    }
    RtcDateTime now = Rtc.GetDateTime();
    showdate(now);
    showalarm();
    VOICE.print(voice);
    RGBSPEED.print(rgbspeed);
     if(alarmswitch==true)
         {ALARMSWITCH.print("on"); }
     else
         {ALARMSWITCH.print("off"); }
     ALARMH.print(alarmh);
     ALARMM.print(alarmm);
}
