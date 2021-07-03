#include  "ssd1306.h"

unsigned char printline=0;

lc_size_t lc_ssd1306_PrintLf(char *string)
{
    unsigned char i=0;
    if(printline==4)
    {
        Oled_ClearFull();
        printline=0;
    }
        while(*string)
    {
        ShowASCII_8x16(i*8,printline,ASCII_8X16[*string-32]);
        string++;
        i++;
        if(i==16){
            printline++;
            i=0;
        }
    }
    printline++;
}

lc_size_t lc_ssd1306_Print(unsigned char x,unsigned char y,const  char *string)
{
    unsigned char i=0;
    while(*string)
    {
        ShowASCII_8x16(x+i*8,y,ASCII_8X16[*string-32]);
        string++;
        i++;	
    }
}

lc_size_t lc_ssd1306_sync(unsigned char *GDDRAM)
{
    unsigned int j;
    WriteCmd(0x21);   
    WriteCmd(0);
    WriteCmd(127);
    WriteCmd(0x22);
    WriteCmd(0);
    WriteCmd(7);
    for(j=0;j<1024;j++)
    {
        WriteDat(*GDDRAM);
        GDDRAM++;
    } 
}

lc_size_t lc_ssd1306_clear()
{
    unsigned int j;
    WriteCmd(0x21);   
    WriteCmd(0);
    WriteCmd(127);
    WriteCmd(0x22);
    WriteCmd(0);
    WriteCmd(7);
    for(j=0;j<1024;j++)
    {
            WriteDat(0x00);
    } 
    printline=0;
}

lc_size_t ssd1306fb_init(void)
{
    unsigned char x,y;
    
    /*
    * 以下为SSD1306设置初始化代码，无需改动
    */
    
    WriteCmd(Oled_Set_Display_Off);	//关显示

    WriteCmd(0xd5);	//设置显示时钟分频率、振荡器频率
    WriteCmd(0x80);//A[3:0]:分频频率1到16，A[7:4]频率

    WriteCmd(0xa8);	//duty设置
    WriteCmd(0x3f);	//duty=1/64

    WriteCmd(0xd3);	//显示偏移
    WriteCmd(0x00); //不偏移
    
    /*
     * ssd1602 user config
     */

    WriteCmd(0x40);	//起始行 40~7F

    WriteCmd(0x8d);	//电荷泵设置
    WriteCmd(0x14); //10:关闭 14:开启

    WriteCmd(0x20);	//设置内存地址模式
    WriteCmd(0x00); //00水平地址模式，01垂直模式，02页地址模式，
 
    WriteCmd(0xc8); //行扫描顺序：从上到下c8	//上下颠倒c0
    WriteCmd(0xa1); //列扫描顺序：从左到右a1	//左右翻转a0

    WriteCmd(0xda); //设置COM口硬件引脚配置
    WriteCmd(0x12);

    WriteCmd(0x81); //微调对比度,本指令的0x81不要改动，改下面的值
    WriteCmd(0x7f); //微调对比度的值，可设置范围0x00～0xff

    WriteCmd(0xd9); //设置预充电周期
    WriteCmd(0xf1); // 

    WriteCmd(0xdb); //Set VCOMH Deselect Level
    WriteCmd(0x30);

    WriteCmd(0xa4); //全局显示开启:a5开启; a4关闭
    WriteCmd(0xa6); //设置显示方式:a7,反相显示;a6,正常显示	

    WriteCmd(Oled_Set_Display_On); //开显示

    Oled_ClearFull();

}

lc_size_t WriteDat(unsigned char IIC_Data)
{
    I2c1_Start();
    I2c1_Sadd(0x78);
    I2c1_Sdata(0x40);       //write data
    I2c1_Sdata(IIC_Data);
    I2c1_Stop();
}


lc_size_t WriteCmd(unsigned char IIC_Command)
{
    I2c1_Start();
    I2c1_Sadd(0x78);      //Slave address,SA0=0
    I2c1_Sdata(0x00);     //write command
    I2c1_Sdata(IIC_Command);
    I2c1_Stop();
}

lc_size_t ShowASCII_8x16(unsigned char x,unsigned char y,const unsigned char *word)
{
    unsigned char j;
    WriteCmd(0x21);   
    WriteCmd(x);
    WriteCmd(x+7);
    WriteCmd(0x22);
    WriteCmd(y*2);
    WriteCmd(y*2+1);
    for(j=0;j<16;j++)
    {
        WriteDat(*word);
        word++;
    } 
}

