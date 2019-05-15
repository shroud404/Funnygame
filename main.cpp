#include "mbed.h"
#include "Adafruit_TCS34725.h"
#include "VL6180x.h"

//VL6180X defines***************
#define VL6180X_ADDRESS 0x29
VL6180x  TOF_sensor(D14,D15,VL6180X_ADDRESS<<1);
//******************************

//tcs34725defines**********************
I2C i2c(A4, A5);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(&i2c, TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_60X);
//***************************************

//上位机uart通信115200
Serial android(PC_12,PD_2,115200);
//*************************
//card defines************
DigitalIn gate0(PC_9);DigitalIn gate1(PC_8);DigitalIn gate2(PC_6);DigitalIn gate3(PC_5);
//***********************
RawSerial pc(USBTX, USBRX);
DigitalOut led(LED1);
BusOut Bus(D6,D5,D4,D3,D2);
//colour函数，返回int 1红色2黑色3蓝色；
int colour()
{ 
        wait(2);
        uint16_t clear, red, green, blue;
        tcs.setInterrupt(false);      // turn on LED
        tcs.getRawData(&red, &green, &blue, &clear);
        tcs.setInterrupt(true);  // turn off LED
            
        int color;
        if((red>blue)&&(red>green)&&(red-green>500))
            color=1;
        else if((red<600)&&(green<600)&&(blue<400)) 
            color= 2;
        else
            color=3;    
        return color;
    }
//************************************

//距离传感器 return uint8_t距离mm**********
uint8_t distant()
{   uint8_t dist;
    int error;  
    error = TOF_sensor.VL6180xInit();  
    if(error!= 0)
    {  
      TOF_sensor.VL6180xDefautSettings();   
    }
    while(1)
    {
        dist = TOF_sensor.getDistance();
        wait(0.5); 
        return dist;}
}
//***********************************

//读卡函数card***********************
int card()
{
    int mode;
    if(gate0==1)
    {
        if(gate1==1&&gate2==1&&gate3==0)
            mode=1;//easy  1 110
        else if(gate1==0&&gate2==0&&gate3==1)
            mode=2;//normal 1 001
        else if(gate1==0&&gate2==1&&gate3==0) 
            mode=3;//hard 1 010
        else if(gate1==0&&gate2==1&&gate3==1)
            mode=4;//地狱 1 011
        }
    else
       mode=0;
    return mode;
    }
//****************************
//上位机通信字符
char androidget()
{
    char c =android.getc();
    return c;
    }
//****************************************

//发送难度模式*************************
void sendmode()
{
    while(1){
                int color=colour();//颜色判断1红色2黑色3蓝色
                uint8_t dist=distant();//距离数值mm
                int mode=card();//card的模式1easy2normal3hard
                char c=androidget();
                if(c=='s')               
                    switch(mode){
                        case 1:android.puts("e");break;
                        case 2:android.puts("n");break;
                        case 3:android.puts("h");break;
                        default:android.puts("p");break;}
                if(c=='q')
                    break;
                }
}
//*********************************





//维护模式****************************************
void maintain()
{    
    
    android.printf("welcome!");
    while(1)
    {
    int color=colour();//颜色判断1红色2黑色3蓝色
    uint8_t dist=distant();//距离数值mm
    int mode=card();//card的模式1easy2normal3hard
    wait(1);
    char c = androidget();//上位机字符
    switch(c)
    {
        case 'a':{
                    switch(color){
                        case 1:android.printf("红色!\n");break;
                        case 2:android.printf("黑色!\n");break;
                        case 3:android.printf("蓝色!\n");break;
                        default:android.printf("错误!\n");break; }
                    };break;//颜色
        case 'b':android.printf("d=%d\n", dist);break;//距离
        case 'c':{            
                    switch(mode){
                        case 1:android.printf("easy!\n");break;
                        case 2:android.printf("normal!\n");break;
                        case 3:android.printf("hard!\n");break;
                        default:android.printf("请插卡!\n");break;}
                    };break;//card
        case 'd':Bus=0b01111;break;//步进电机
        case 'e':Bus=0b10010;break;//舵机
        default:break;
        } 
        if(c=='q')
            break;
        }  
    }
//*********************************************************
//游戏开始***************************************************
int startgame()
{   
    android.printf("开始游戏！\n");
    wait(1);
    int step=0;
    int backstep=0;
    int i=0;
    while(i<5)
    {
        char c =androidget();
        if(c =='0'|c=='1'|c=='2'|c =='3'|c=='4'|c=='5'|c=='6')
            {step+=(c-48);
            i=i+1;}
        if(step>=16)
            {Bus=(step-16);android.printf("step=%d,backstep=%d\n",step,backstep);break;}
        switch(c)
        {
            case '0':break;
            case '1':Bus=0b00001;break;
            case '2':Bus=0b00010;break;
            case '3':Bus=0b00011;break;
            case '4':Bus=0b00100;break;
            case '5':Bus=0b00101;break;
            case '6':Bus=0b00110;break;
            default:break;
            }
        if(i==5&&step<16)
        {backstep=16-step;
        android.printf("step=%d,backstep=%d\n",step,backstep);
        return backstep;}
        } 
    
    }
//****************************************
//reset***********************************
void reset(int backstep)
{
    android.printf("GUIreset!\n");
    switch(backstep)
    {
            case 0:break;
            case 1:Bus=0b00001;break;
            case 2:Bus=0b00010;break;
            case 3:Bus=0b00011;break;
            case 4:Bus=0b00100;break;
            case 5:Bus=0b00101;break;
            case 6:Bus=0b00110;break;
            case 7:Bus=0b00111;break;
            case 8:Bus=0b01000;break;
            case 9:Bus=0b01001;break;
            case 10:Bus=0b01010;break;
            case 11:Bus=0b01011;break;
            case 12:Bus=0b01100;break;
            case 13:Bus=0b01101;break;
            case 14:Bus=0b01110;break;
            case 15:Bus=0b01111;break;
            case 16:Bus=0b01000;break;
            default:break;
            };
    
    led=0;
    }
//******************************************
//award*************************************
void award()
{
    android.printf("GUIaward!\n");
while(1){
    int c=card();
    
    int color;//颜色判断1红色2黑色3蓝色
    if(c==1){Bus=((color=colour())==1)?0b10000:0b10001;
                wait(3);
               Bus=((color=colour())==1)?0b10000:0b10010;
               break;}   //red--easy 
    if(c==2){Bus=((color=colour())==2)?0b10000:0b10001;
                wait(3);
               Bus=((color=colour())==2)?0b10000:0b10010;
               android.printf("heisejiangpin");
               break;} //black==normal
    if(c==3){Bus=((color=colour())==3)?0b10000:0b10001;
                wait(3);
               Bus=((color=colour())==3)?0b10000:0b10010;
               break;}  //blue--hard
               }                
    }
//*****************************************
int main()
{       
 while(1){ 
        led=0;
        android.printf("hello!");
        char c=androidget();
        
        if(c=='m')  
            {
            android.printf("维护模式！\n");
            wait(0.2);
            maintain();}        
        else if(c=='o')
           {
                android.printf("进入游戏界面!\n");
                int color=colour();//颜色判断1红色2黑色3蓝色
                uint8_t dist=distant();//距离数值mm
                int mode=card();//card的模式1easy2normal3hard               
                led=1;
                if(dist<200)//15cm内
                {
                    int bstep=0;
                    led=0;
                    sendmode();
                    wait(1);
                    bstep=startgame();
                    wait(5);                   
                    android.printf("NEXT!\n");
                    award();
                    android.printf("NEXT1!\n");
                    reset(bstep);
                    wait(5);
                    }
                else
                {   
                    led=0;
                    android.puts("k");
                    android.printf("请靠近机器人!\n");
                    }         
                }      
            }           
}

