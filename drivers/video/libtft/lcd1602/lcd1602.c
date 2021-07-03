#include "lcd1602.h"
uchar led =1;
void Lcd1602_WaitReady()
{
	uint null =0;
	while(1==((Lcd1602_Read(0)>>7)&0x01))
	{
		null++;
		if(null==0xffff)return;
	}
}
void Lcd1602_Cmd(uchar cmd)
{
	Lcd1602_WaitReady();
	Lcd1602_Write(0,cmd);
}
void Lcd1602_Datax(uchar data)
{
	Lcd1602_WaitReady();
	Lcd1602_Write(1,data);
}
void Lcd1602_Init()
{
   Lcd1602_Cmd(0x28);
   Lcd1602_Cmd(0x0c);
   Lcd1602_Cmd(0x01);
   Lcd1602_Cmd(0x08);
}

void Lcd1602_Print(unsigned char ex,unsigned char ey,char *ei)
{
	if(ey==0)
	{
		Lcd1602_Cmd(0x80+ex);
	}
	else
	{
		Lcd1602_Cmd(0xc0+ex);
	}
	while(*ei)	
	{
			Lcd1602_Datax(*ei);
			ei++;
	}
		
}
void Lcd1602_BackLight(uchar sw)
{
	led=sw;
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD);
	I2c1_Sdata(0xF7+(sw<<3));
	I2c1_Stop();
}
void Lcd1602_Write(uchar rs,uchar data)
{
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD);
	I2c1_Sdata((data&0xf0)|(led<<3)|(1<<2)|(0<<1)|(rs<<0));//写入数据高4位
	I2c1_Sdata((data&0xf0)|(led<<3)|(0<<2)|(0<<1)|(rs<<0));//拉低e,数据生效
	I2c1_Sdata((data<<4)	|(led<<3)|(1<<2)|(0<<1)|(rs<<0));
	I2c1_Sdata((data<<4)	|(led<<3)|(0<<2)|(0<<1)|(rs<<0));
	I2c1_Sdata((data<<4)	|(led<<3)|(0<<2)|(0<<1)|(rs<<0));
	I2c1_Stop();
}
uchar Lcd1602_Read(uchar rs)
{
	uchar temp =0;
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD);
	I2c1_Sdata((0xf0)|(led<<3)|(1<<2)|(1<<1)|(rs<<0));//象征性的拉高e
	I2c1_Stop();
	
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD+1);
	temp |= I2c1_Rdata()&0xf0;
	I2c1_Stop();
	
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD);
	I2c1_Sdata((0xf0)|(led<<3)|(0<<2)|(1<<1)|(rs<<0));
	I2c1_Stop();
	
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD);
	I2c1_Sdata((0xf0)|(led<<3)|(1<<2)|(1<<1)|(rs<<0));
	I2c1_Stop();
	
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD+1);
	temp |= I2c1_Rdata()>>4;
	I2c1_Stop();
	
	I2c1_Start();
	I2c1_Sadd(Lcd1602_ADD);
	I2c1_Sdata((0xf0)|(led<<3)|(0<<2)|(1<<1)|(rs<<0));
	I2c1_Stop();
	return temp;
}
