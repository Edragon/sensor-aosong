/***********************************************
文件名:RT12232F.h
功能:RT12232F-2 液晶串行驱动头文件
应用平台：STM8 COSMIC
版本:2.0
创建日期:2010/10/15
最后修改：zhshliu
***********************************************/
 
#ifndef _HT1621_H_
#define _HT1621_H_

#include "ddl.h"
#include "gpio.h"

/*********************HW***********************/
//如脚位有改变请改变以对应的IO口
//WR-->PC7 DATA-->PC6 CS-->PC5 
#define	  LCD_GPIO_nCS  (GPIOC)
#define	  LCD_GPIO_WR	(GPIOC)
#define	  LCD_GPIO_SDA  (GPIOC)
//片选线
#define   LCD_nCS       (1<<5)
//时钟线
#define   LCD_WR        (1<<7)
//数据线
#define   LCD_SDA       (1<<6)
	
#define	  SET_nCS		Gpio_SetIO(2,5,1) //(LCD_GPIO_nCS->ODR |= LCD_nCS)
#define	  CLR_nCS		Gpio_SetIO(2,5,0) //(LCD_GPIO_nCS->ODR &= ~LCD_nCS)

#define	  SET_WR		Gpio_SetIO(3,4,1) //(LCD_GPIO_WR->ODR |= LCD_WR)
#define	  CLR_WR		Gpio_SetIO(3,4,0) //(LCD_GPIO_WR->ODR &= ~LCD_WR)

#define	  SET_SDA		Gpio_SetIO(2,6,1) //(LCD_GPIO_SDA->ODR |=  LCD_SDA)
#define	  CLR_SDA		Gpio_SetIO(2,6,0) //(LCD_GPIO_SDA->ODR &= ~LCD_SDA)	
/*********************HW***********************/
/*
HT1621.DisBuf[10] = 0x01;		//d
HT1621.DisBuf[10] = 00000010;		//e
HT1621.DisBuf[10] = 00000100;		//f
HT1621.DisBuf[10] = 00001000;		//a
HT1621.DisBuf[10] = 00010000;		//s1
HT1621.DisBuf[10] = 00100000;		//c
HT1621.DisBuf[10] = 01000000;		//g
HT1621.DisBuf[10] = 10000000;		//b

HT1621.DisBuf[5] = 0x01;		    //%
HT1621.DisBuf[5] = 00000010;		//b
HT1621.DisBuf[5] = 00000100;		//g
HT1621.DisBuf[5] = 00001000;		//c
HT1621.DisBuf[5] = 00010000;		//a
HT1621.DisBuf[5] = 00100000;		//f
HT1621.DisBuf[5] = 01000000;		//e
HT1621.DisBuf[5] = 10000000;		//d
*/
#define DISPLAY_N 20
#define DISPLAY_Y 21
#define DISPLAY_T 22
#define DISPLAY_H 23
#define DISPLAY_O 24
#define DISPLAY_V 25
#define DISPLAY_A 26

typedef struct{
	unsigned char DisBuf[12];
	uint8_t Start;
	uint8_t   PowerOn;
	uint8_t MenuProcess_flag;
	uint8_t CelSignEnable;
	uint8_t RHSignEnable;
	uint8_t PercentSignEnable;
	uint8_t LineSignEnable;
	uint8_t   BatValue;
	uint8_t PowerOff_flag;
}HT1621_Struct;

extern HT1621_Struct HT1621;

void Init_LCD(void);
void DisplayCaltemp(signed char IDBuf);
void DisplayCalhumi(signed char IDBuf);
void DisplayReset(uint8_t IDBuf);
void DisplayBattery(uint8_t IDBuf);
void DisplayAutoSleep(signed char IDBuf);
void DisplayVersion(void);

void Display_Data(int Val1,int Val2);
void Display(void);
void Display_Off(void);
void HT1621_LCD_ON(void);
void HT1621_LCD_OFF(void);

void Disy(void);
void qing(void);

void Display_F(int f);
void Display_T(int t);
void Display_RH(int rh);
	

#endif //_HT1621_H_
