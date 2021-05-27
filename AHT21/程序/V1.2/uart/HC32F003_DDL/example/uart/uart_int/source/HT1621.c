#include "HT1621.h"
//#include "DHT11.h"
//#include "key.h"
//#include "menu.h"

HT1621_Struct HT1621;
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

/*
0	 1	  2    3	4	  5	    6	  7	    8	  9	    A	  B	    C	  D	    E	  F	    r	  NL
0xAF,0xA0,0xCB,0xE9,0xE4, 0x6D, 0x6F, 0xA8, 0xEF, 0xED, 0xEE, 0x67, 0x0F, 0xE3, 0x4F, 0x4E, 0x42, 0x00
n	 y	  t    h	o          -
0x62,0xE5,0x47,0x66,0x63   0x40
0x4C,0xAE,0xE4,0x6C,0xCC
*/
 static const unsigned char seg_tab123[27]=
{
	0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED,	//0,1,2,3,4,5,6,7,8,9
	0xCE,0x67,0x0F,0xE3,0x4F,0x4E,0x42,0x00,0x40,0X46,				//P,b,C,d,E,F,r,NL,-
	0x62,0xE5,0x47,0xE6,0x63,0xA7,0xEE                             //n	 y	  t    H	o  V  A
};
/*
0	 1	  2    3	4	  5	    6	  7	    8	  9	    A	  B	    C	  D	    E	  F	    r	  NL
0xFA,0x0A,0xD6,0x9E,0x2E, 0x7C, 0xFC, 0x1A, 0xFE, 0xBE, 0x7E, 0xEC, 0xF0, 0xCE, 0xF4, 0x74, 0x44, 0x00
*/
static const unsigned char seg_tab456[27]=
{
	0xFA,0x0A,0xD6,0x9E,0x2E,0xBC,0xFC,0x1A,0xFE,0xBE,	//0,1,2,3,4,5,6,7,8,9
	0x76,0xEC,0xF0,0xCE,0xF4,0x74,0x44,0x00,0x04,0x00,				//P,b,C,d,E,F,r,NL,-
	0x7A,0xAE,0xE4,0x6E,0xCC,0xEA,0x7E                               //n	 y	  t    H	o  V   A
};

//华氏度切换
/*const unsigned char seg_tab789[]= {
	

};*/

/***********************************************/
//1621的显存是4位*32的,故用16*8位的寄存器就能满足要求奇数显存放高4位,偶数放低4位
/*****************************************************************/

/*
//1621初始化命令
const unsigned char Init_Tab[10]=
{
	0b00000001,//打开系统振荡器命令
	0b00000011,//打开LCD偏压发生器命令
	0b00000101,//WDT 溢出标志输出失效命令
	0b00000110,//时基输出使能命令
	//0b00011000,//系统时钟源片内RC振荡器命令
	0b00010100,//系统时钟源片外部振荡器命令
	0b00101001,//LCD1/3偏压选项4个公共口命令
	//0b01000000,//声音频率4KHz命令
	//0b00001001,//打开声音输出命令
	0b10100000,//时基/WDT 时钟输出1Hz命令
	0b10001000 //使/IRQ 输出有效命令
};*/


void SET_ALL_PINS_OUTPP(void)
{
    /**********LCD_CS***********/
//	LCD_GPIO_nCS->ODR |= LCD_nCS;			//输出'1'
//	LCD_GPIO_nCS->DDR |= LCD_nCS;			//输出模式
//	LCD_GPIO_nCS->CR1 |= LCD_nCS;			//推挽模式
//	LCD_GPIO_nCS->CR2 |= LCD_nCS;			//高速输出
	
	Gpio_InitIOExt(2,5,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P15 to output
	Gpio_SetIO(2,5,1); 

    /**********LCD_WR***********/
//	LCD_GPIO_WR->ODR  |= LCD_WR;			//输出'1'
//	LCD_GPIO_WR->DDR  |= LCD_WR;			//输出模式
//	LCD_GPIO_WR->CR1  |= LCD_WR;			//推挽模式
//	LCD_GPIO_WR->CR2  |= LCD_WR;			//高速输出
	Gpio_InitIOExt(3,4,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P15 to output
	Gpio_SetIO(3,4,1); 

    /*********LCD_DATA**********/
//	LCD_GPIO_SDA->ODR |= LCD_SDA;			//输出'1'
//	LCD_GPIO_SDA->DDR |= LCD_SDA;			//输出模式
//	LCD_GPIO_SDA->CR1 |= LCD_SDA;			//推挽模式
//	LCD_GPIO_SDA->CR2 |= LCD_SDA;			//高速输出
  Gpio_InitIOExt(2,6,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P15 to output
	Gpio_SetIO(2,6,1); 
}

void SET_ALL_PINS_INFL(void)
{
    /**********LCD_CS***********/
//	LCD_GPIO_nCS->DDR &= ~(LCD_nCS);			//输入模式
//	LCD_GPIO_nCS->CR1 &= ~(LCD_nCS);			//浮空模式
//	LCD_GPIO_nCS->CR2 &= ~(LCD_nCS);			//无中断

//    /**********LCD_WR***********/
//	LCD_GPIO_WR->DDR &= ~(LCD_WR);			//输入模式
//	LCD_GPIO_WR->CR1 &= ~(LCD_WR);			//浮空模式
//	LCD_GPIO_WR->CR2 &= ~(LCD_WR);			//无中断

//    /*********LCD_DATA**********/
//	LCD_GPIO_SDA->DDR &= ~(LCD_SDA);			//输入模式
//	LCD_GPIO_SDA->CR1 &= ~(LCD_SDA);			//浮空模式
//	LCD_GPIO_SDA->CR2 &= ~(LCD_SDA);			//无中断
	
	Gpio_InitIO(2, 5, GpioDirIn);
	Gpio_InitIO(3, 4, GpioDirIn);
	Gpio_InitIO(2, 6, GpioDirIn);
}

void Init_LCDPORT(void)
{
	SET_ALL_PINS_OUTPP();
}

//延时t个微秒
void delay_us(unsigned int t)
{
	while(--t)
	{
		;
	}
}

void Send_Data(unsigned char data_temp,unsigned char loop)
{
	for(;loop!=0;loop--)
	{
		delay_us(1);
		CLR_WR;
		delay_us(1);
		if((data_temp&0x80)==0)  //0B10000000
		{
			CLR_SDA;
		}
		else
		{
			SET_SDA;
		}
		data_temp=(data_temp<<1);
		SET_WR;
	}
}

void Init_HT1621(void)
{
	//unsigned char t;
	//拉高SDA,CS，SCL
	SET_ALL_PINS_OUTPP();
	CLR_nCS;
	Send_Data(0x80,3);//送3位命令模式代码100 0b10000000
	//送9位命令代码 
	Send_Data(0x01,9);//0b00000001,//打开系统振荡器命令
	Send_Data(0x03,9);//0b00000011,//打开LCD偏压发生器命令
	Send_Data(0x05,9);//0b00000101,//WDT 溢出标志输出失效命
	Send_Data(0x06,9);//0b00000110,//时基输出使能命令
	//Send_Data(0x14,9);//0b00010100,//系统时钟源片外部振荡器命令
	Send_Data(0x18,9);//0b00011000,//系统时钟源片内RC振荡器命令
	Send_Data(0x29,9);//0b00101001,//LCD1/3偏压选项4个公共口命令
	Send_Data(0xa0,9);//0b10100000,//时基/WDT 时钟输出1Hz命令
	Send_Data(0x88,9);//0b10001000 //使/IRQ 输出有效命令
	
	//0b00011000,//系统时钟源片内RC振荡器命令
	//0b01000000,//声音频率4KHz命令
	//0b00001001,//打开声音输出命令
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
}


void HT1621_LCD_ON(void)
{
 SET_ALL_PINS_OUTPP();
	CLR_nCS;
	Send_Data(0x80,3);//送3位命令模式代码100 0b10000000
	
	Send_Data(0x01,9);//0b00000001,//打开系统振荡器命令
	Send_Data(0x03,9);//0b00000011,//打开LCD偏压发生器命令
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
 }	

 
 void HT1621_LCD_OFF(void)
{
 SET_ALL_PINS_OUTPP();
	CLR_nCS;
	Send_Data(0x80,3);//送3位命令模式代码100 0b10000000
	
	Send_Data(0x00,9);//0b00000001,//关系统振荡器命令
	Send_Data(0x02,9);//0b00000011,//关LCD偏压发生器命令
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
 }	
/******************************
name:init_lcd
function:Initialize lcd
prams :null
return :null
******************************/
//extern uint8_t  gFlashCount; 	   //菜单闪烁控制计数
void Init_LCD(void)
{
	Init_LCDPORT();
	Init_HT1621();

	HT1621.Start = 0;//开机显示ID 2S

	HT1621.CelSignEnable = 1;				//打开摄氏度图标
	HT1621.DisBuf[4] |= 0x01;//横线
}



/********************************************************************************************/

void Display_F(int f)
{
	uint16_t Temp1;
	
	if (1)//Sensor.Power_Value > 20
	{
		if(1)//!Sensor.Error
		{
			/**/
			if((f > 30218)||(f < -6700)) //超出 
			{
				HT1621.DisBuf[10] = seg_tab123[0x0E];		//Err
				HT1621.DisBuf[9]  = seg_tab123[0x10];
				HT1621.DisBuf[8]  = seg_tab123[0x10];
			}
			
			//    正常显示华氏度 X.X或者XX.X
			else if(f >9 && f <= 9999)	
			{
				//提取千位，百位，十位
				HT1621.DisBuf[10] = seg_tab123[f/1000%10];
				HT1621.DisBuf[9]  = seg_tab123[f/100%10]|0x10;		//Dot
				HT1621.DisBuf[8]  = seg_tab123[f/10%10]|0x10;		//显示F标志
				//当第一段数码管为0，把0去掉
				if(HT1621.DisBuf[10] == (seg_tab123[0]))//零消隐
				{
					HT1621.DisBuf[10] = 0x0;
				}
			}
			
			//判断华氏度接近0的时候，一律按0计算
			else if(f > 0 && f <= 9)
			{
				HT1621.DisBuf[10] = 0x0;
				HT1621.DisBuf[9]  = 0x0;		
				HT1621.DisBuf[8]  = seg_tab123[0] |0x10;		//显示F标志
			}
			
			//高于37.7度，即99.9，不用显示小数点了
			else if(f>9999)				
			{
				HT1621.DisBuf[10] = seg_tab123[f/10000];
				HT1621.DisBuf[9] = seg_tab123[f/1000%10];
				HT1621.DisBuf[8]  = seg_tab123[f/100%10]|0x10;;		//显示F标志					
			}
			
			//显示0-X的数据
			else if(f <= -4 && f >= -9)   // 显示负数情况下的华氏度
			{
				Temp1 = (unsigned int)(0-f);
				HT1621.DisBuf[10] = seg_tab123[18];
				HT1621.DisBuf[9]  = seg_tab123[f/100%10];		
				HT1621.DisBuf[8]  = seg_tab123[Temp1] |0x10;		//显示F标志	//显示F标志	
			}
			//显示-0.1 ~ -9.9的数据
			else if(f <= -10 && f >= -999)
			{
				Temp1 = (unsigned int)(0-f);
				//提取千位，百位，十位
				HT1621.DisBuf[10] = seg_tab123[Temp1/1000%10];
				HT1621.DisBuf[9]  = seg_tab123[Temp1/100%10]|0x10;		//Dot
				HT1621.DisBuf[8]  = seg_tab123[Temp1/10%10]|0x10;		//显示F标志
				//当第一段数码管为0，把0去掉
				if(HT1621.DisBuf[10] == (seg_tab123[0]))//显示“-”号
				{
					HT1621.DisBuf[10] = seg_tab123[18];
				}
			}
			//显示-10.0 ~ -50 
			else if(f < -999 && f >= -5000)         
			{
				Temp1 = (unsigned int)(0-f);
				//提取千位，百位，十位
				HT1621.DisBuf[10] = seg_tab123[18];
				HT1621.DisBuf[9]  = seg_tab123[Temp1/1000%10];		
				HT1621.DisBuf[8]  = seg_tab123[Temp1/100%10]|0x10;		//显示F标志
			}		
		}
		
		else//传感器错误
		{
			HT1621.DisBuf[10] = seg_tab123[0x0E];
			HT1621.DisBuf[9]  = seg_tab123[0x10];
			HT1621.DisBuf[8]  = seg_tab123[0x10];
			
			HT1621.DisBuf[5]  = seg_tab456[0x0E];
			HT1621.DisBuf[6]  = seg_tab456[0x10];
			HT1621.DisBuf[7]  = seg_tab456[0x10];
		}
	}
}


void Display_T(int t)
{
	uint16_t Temp1;
	if (1)//Sensor.Power_Value > 20
	{
		if(1)//!Sensor.Error
		{
			if((t >= 1501)||(t < -550)) //超出 550
			{
				HT1621.DisBuf[10] = seg_tab123[0x0E];		//Err
				HT1621.DisBuf[9]  = seg_tab123[0x10];
				HT1621.DisBuf[8]  = seg_tab123[0x10];
			}
			else if(t >= 0)	//0~99.9度
			{
				Temp1 = (unsigned int)t;
				HT1621.DisBuf[10] = seg_tab123[Temp1/100%10]|0x10;
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10]|0x10;//Dot
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];
				if(HT1621.DisBuf[10] == (seg_tab123[0]|0x10))//零消隐
					HT1621.DisBuf[10] = 0x10;
				
				if(Temp1>999)
				{
				 HT1621.DisBuf[10] = seg_tab123[Temp1/1000]|0x10;;
				 HT1621.DisBuf[9]  = seg_tab123[Temp1/100%10];//Dot
				 HT1621.DisBuf[8]  = seg_tab123[Temp1/10%100%10];
				 if(HT1621.DisBuf[10] == (seg_tab123[0]|0x10))//零消隐
				 HT1621.DisBuf[10] = 0x10;
				 }
			}
			else if((0 > t)&&( t > -100))//-x.x
			{
				Temp1 = (unsigned int)(0-t);
				HT1621.DisBuf[10] = seg_tab123[18]|0x10;		//-
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10]|0x10;//Dot
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];
			}
			else 			//-xx
			{
				Temp1 = (unsigned int)((0-t)/10);  //转为正数，并除以10
				HT1621.DisBuf[10] = seg_tab123[18]|0x10;//显示负号-
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10];//显示十位
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];//显示个位
			}
		}			
			
		else//传感器错误
		{
			HT1621.DisBuf[10] = seg_tab123[0x0E];
			HT1621.DisBuf[9]  = seg_tab123[0x10];
			HT1621.DisBuf[8]  = seg_tab123[0x10];
			
			HT1621.DisBuf[5]  = seg_tab456[0x0E];
			HT1621.DisBuf[6]  = seg_tab456[0x10];
			HT1621.DisBuf[7]  = seg_tab456[0x10];
		}
	}

}


void Display_RH(int rh)
{
	uint16_t Temp1;
	if (1)//Sensor.Power_Value > 20
	{
		if(1)//!Sensor.Error
		{
			if((rh >= 1000)||(rh <= 0)) //超出
			{
				HT1621.DisBuf[5] =seg_tab456[9];// seg_tab456[0x0E];
				HT1621.DisBuf[6] =seg_tab456[9];// seg_tab456[0x10];
				HT1621.DisBuf[7] =seg_tab456[9];// seg_tab456[0x10];
			}
			else	//0~99.9RH
			{ 
				
				
				Temp1 = (unsigned int)rh;
				
				HT1621.DisBuf[5] = seg_tab456[Temp1/100%10]|0x01;
				HT1621.DisBuf[6] = seg_tab456[Temp1/10%10]|0x01;//Dot
				HT1621.DisBuf[7] = seg_tab456[Temp1%10]|0x01;
				
				if(HT1621.DisBuf[5] == (seg_tab456[0]|0x01))//零消隐
					HT1621.DisBuf[5] = 0x01;
			}
		}
		else//传感器错误
		{
			HT1621.DisBuf[10] = seg_tab123[0x0E];
			HT1621.DisBuf[9]  = seg_tab123[0x10];
			HT1621.DisBuf[8]  = seg_tab123[0x10];
			
			HT1621.DisBuf[5]  = seg_tab456[0x0E];
			HT1621.DisBuf[6]  = seg_tab456[0x10];
			HT1621.DisBuf[7]  = seg_tab456[0x10];
		}
	}
}

/********************************************************************************************/


void Display_Data(int Val1,int Val2)
{
	uint16_t Temp1;
	if (1)//Sensor.Power_Value > 20
	{
		if(1)//!Sensor.Error
		{
			//数据1
			/**/
			if((Val1 >= 1501)||(Val1 < -550)) //超出 550
			{
				HT1621.DisBuf[10] = seg_tab123[0x0E];		//Err
				HT1621.DisBuf[9]  = seg_tab123[0x10];
				HT1621.DisBuf[8]  = seg_tab123[0x10];
			}
			else if(Val1 >= 0)	//0~99.9度
			{
				Temp1 = (unsigned int)Val1;
				HT1621.DisBuf[10] = seg_tab456[Temp1/100%10]|0x10;
				HT1621.DisBuf[9]  = seg_tab456[Temp1/10%10]|0x10;//Dot
				HT1621.DisBuf[8]  = seg_tab456[Temp1%10];
				if(HT1621.DisBuf[10] == (seg_tab456[0]|0x10))//零消隐
					HT1621.DisBuf[10] = 0x10;
				
				if(Temp1>999)
				{
				 HT1621.DisBuf[10] = seg_tab123[Temp1/1000]|0x10;;
				 HT1621.DisBuf[9]  = seg_tab123[Temp1/100%10];//Dot
				 HT1621.DisBuf[8]  = seg_tab123[Temp1/10%100%10];
				 if(HT1621.DisBuf[10] == (seg_tab123[0]|0x10))//零消隐
				 HT1621.DisBuf[10] = 0x10;
				 }
			}
			else if((0 > Val1)&&( Val1 > -100))//-x.x
			{
				Temp1 = (unsigned int)(0-Val1);
				HT1621.DisBuf[10] = seg_tab123[18]|0x10;		//-
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10]|0x10;//Dot
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];
			}
			else 			//-xx
			{
				Temp1 = (unsigned int)((0-Val1)/10);  //转为正数，并除以10
				HT1621.DisBuf[10] = seg_tab123[18]|0x10;//显示负号-
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10];//显示十位
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];//显示个位
			}  
			
			//数据2
			if((Val2 >= 1000)||(Val2 <= 0)) //超出
			{
				HT1621.DisBuf[5] =seg_tab456[9];// seg_tab456[0x0E];
				HT1621.DisBuf[6] =seg_tab456[9];// seg_tab456[0x10];
				HT1621.DisBuf[7] =seg_tab456[9];// seg_tab456[0x10];
			}
			else	//0~99.9RH
			{ 
				
				
				Temp1 = (unsigned int)Val2;
				
				HT1621.DisBuf[5] = seg_tab456[Temp1/100%10]|0x01;
				HT1621.DisBuf[6] = seg_tab456[Temp1/10%10]|0x01;//Dot
				HT1621.DisBuf[7] = seg_tab456[Temp1%10]|0x01;
				
				if(HT1621.DisBuf[5] == (seg_tab456[0]|0x01))//零消隐
					HT1621.DisBuf[5] = 0x01;
			}
		}
		else//传感器错误
		{
			HT1621.DisBuf[10] = seg_tab123[0x0E];
			HT1621.DisBuf[9]  = seg_tab123[0x10];
			HT1621.DisBuf[8]  = seg_tab123[0x10];
			
			HT1621.DisBuf[5]  = seg_tab456[0x0E];
			HT1621.DisBuf[6]  = seg_tab456[0x10];
			HT1621.DisBuf[7]  = seg_tab456[0x10];
		}
	}
	


}

void Display(void)
{
	unsigned char t;
	unsigned char seg_temp;
  
	SET_ALL_PINS_OUTPP();
	CLR_nCS;
	CLR_WR;
	Send_Data(0xa0,3);//送3位“写 ”模式命令代码101
	Send_Data(0,6);//送6位首地址00h
	for(t=0;t<=10;t++)
	{
		seg_temp =HT1621.DisBuf[t];
		Send_Data(seg_temp,8);//向显存送数据		
	}
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
}

void Disy(void)
{
	unsigned char t;
	unsigned char seg_temp;
  
	SET_ALL_PINS_OUTPP();
	CLR_nCS;
	CLR_WR;
	Send_Data(0xa0,3);//送3位“写 ”模式命令代码101
	Send_Data(0x9<<2,6);//送6位首地址00h
	
	seg_temp =HT1621.DisBuf[1];
	Send_Data(0x1f,4);//向显存送数据		
	
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
}


void qing(void)
{
	unsigned char t;
	unsigned char seg_temp;
  
	SET_ALL_PINS_OUTPP();
	CLR_nCS;
	CLR_WR;
	Send_Data(0xa0,3);//送3位“写 ”模式命令代码101
	Send_Data(0x0,6);//送6位首地址00h
	
	seg_temp =HT1621.DisBuf[0];
	Send_Data(0x00,128);//向显存送数据		
	
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
}

//void Display_Off(void)
//{
//	unsigned char t;
//	unsigned char seg_temp;
//  
//	SET_ALL_PINS_OUTPP();
//	CLR_nCS;
//	CLR_WR;
//	Send_Data(0xa0,3);//送3位“写 ”模式命令代码101
//	Send_Data(0,6);//送6位首地址00h
//	for(t=0;t<=10;t++)
//	{
//		seg_temp = 0;
//		Send_Data(seg_temp,8);//向显存送数据		
//	}
//	SET_nCS;
//	SET_WR;
//	SET_ALL_PINS_INFL();
//}


