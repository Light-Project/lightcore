#ifndef _LCD1602_H_
#define _LCD1602_H_
#include "stm32f103.h"
#include "i2c.h"
#define uchar       unsigned char
#define uint        unsigned int
#define ulong       unsigned long
//设备地址设置
#define	Lcd1602_ADD		0x4e
//基本命令
#define Clear_Dispaly 0x01	//清除屏幕
#define Return_Home		0x02	//光标归位
//命令地址
#define	Entry_Mode_Set 0x03	//写入模式设置基址(写入数据左右移,开关移动)
#define	Dispaly_Control 0x08//显示模式设置基址(显示开关,游标开关,游标闪烁)
#define	Cursor_Display	0x10//
#define	function_Set	0x20	
#define	Read_Busy	
//底层函数定义
void Lcd1602_Init();
void Lcd1602_Print(unsigned char ex,unsigned char ey,char *ei);

void Lcd1602_BackLight(uchar sw);
void Lcd1602_Write(uchar rs,uchar data);
uchar Lcd1602_Read(uchar rs);

#endif
