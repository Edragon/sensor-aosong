#include "pcf8562.h"
#include "gpio.h"

/***************************************************************/
//ÎÂ¶ÈÊý/»ªÊÏ¶ÈÂë¹Ü£¬¼´1,2,3
const unsigned char SEG_T[]= {
	0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED,				//0--9
	0x4F,0x42,0x40																						//E  r  -
};

//Êª¶ÈÊýÂë¹Ü£¬¼´4,5,6
const unsigned char SEG_RH[]= {
	0xFA,0x0A,0xD6,0x9E,0x2E,0xBC,0xFC,0x1A,0xFE,0xBE,       //0--9
	0xF4,0X44,0X04																					//E  r  -		
};



/*************************************************************/
void Delay_n10us(uint32_t t)
{
  uint32_t k;

   while(t--) {
    for (k = 0; k < 2; k++)//110
      ;
  }
}

void Delay_us(uint32_t t)
{
	t = t-2;
	
	for(; t>0; t--) {
		Delay_n10us(1);
	}
}
/***************************************************************/

/******************************
IIC¹Ü½Å¶¨Òå
*******************************/
void SDA_High(void)
{
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,5,1);
}

void SDA_Low(void)
{
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,5,0);
}

void SDA_IN_FLOATING(void)
{
	Gpio_InitIO(1, 5, GpioDirIn);
}

void SCL_High(void)
{
	Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1, 4, 1);
}

void SCL_Low(void)
{
	Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1, 4, 0);
}

/*******************
IIC¹Ü½Å³õÊ¼»¯
********************/

void IIC_Init(void)
{
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,5,1);
	
  Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,4,1);
}

//IICÇý¶¯Ê±Ðò
void IIC_Start(void)
{
	SDA_High();
	Delay_us(8);
	SCL_High();
	Delay_us(8);
	SDA_Low();
	Delay_us(8);
	SCL_Low();
	Delay_us(8);   
}

void IIC_WR_Byte(uint8_t Byte)
{
	uint8_t i;	

	for(i=0;i<8;i++)
	{
		SCL_Low();   
		Delay_us(2);	
	
		if(0x80&Byte)
		{
			SDA_High();
		}
		else
		{
			SDA_Low();
		}	

		SCL_High();
		Delay_us(2);
		Byte <<= 1;  
	}
	SCL_Low();
	Delay_us(8);   
	SDA_IN_FLOATING();
	Delay_us(8);	
}

uint8_t IIC_RD_Byte(void)
{
	uint8_t Byte,i,a;
	Byte = 0;
	SCL_Low();
	SDA_IN_FLOATING();
	Delay_us(8);	
	for(i=0;i<8;i++)
	{
    SCL_High();		
		Delay_us(2);
		a=0;
		if(Gpio_GetIO(1,5))
			a=1;
		Byte = (Byte<<1)|a;
		
		SCL_Low();
		
		Delay_us(2);
	}
  SDA_IN_FLOATING();
	Delay_us(8);	
	return Byte;
}

uint8_t IIC_Wait_ACK(void)
{
	uint16_t CNT;
	CNT = 0;
	SCL_Low();	
	SDA_IN_FLOATING();
	Delay_us(8);	
	SCL_High();	
	Delay_us(8);	
	while((Gpio_GetIO(1,5))  && CNT < 100) 
	CNT++;
	
	if(CNT == 100)
	{
		//Read_Error = I2C_Error;
		return 0;
	}
	
 	SCL_Low();	
	Delay_us(8);	
	return 1;
}

void IIC_Send_ACK(void)	
{
	SCL_Low();	
	Delay_us(8);	
	SDA_Low();
	Delay_us(8);	
	SCL_High();	
	Delay_us(8);
	SCL_Low();	
	Delay_us(8);
	SDA_IN_FLOATING();
	Delay_us(8);
}

void IIC_Send_NOT_ACK(void)
{
	SCL_Low();	
	Delay_us(8);
	SDA_High();
	Delay_us(8);
	SCL_High();	
	Delay_us(8);		
	SCL_Low();	
	Delay_us(8);
  SDA_Low();
	Delay_us(8);
}

void IIC_Stop(void)
{
	SDA_Low();
	Delay_us(8);
	SCL_High();	
	Delay_us(8);
	SDA_High();
	Delay_us(8);
}

/**********************
PCF8562²Ù×÷º¯Êý
***********************/
/********************
³õÊ¼»¯PCF8562£¬Ö÷ÒªÊÇ4¸öÃüÁîµÄ·¢ËÍ
********************/
void PCF8562_Init(void)
{
	IIC_Init();
	
	IIC_Start();
	
	//·¢ËÍÃüÁî³õÊ¼»¯
	IIC_WR_Byte(PCF8562);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_1);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_2);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_3);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_4);
	IIC_Wait_ACK();
	
	IIC_WR_Byte(0x10 );
	IIC_Wait_ACK();
	IIC_WR_Byte(0x50);
	IIC_Wait_ACK();
	IIC_WR_Byte(0x50);
	IIC_Wait_ACK();
	IIC_WR_Byte(0x55);
	IIC_Wait_ACK();
	
	IIC_WR_Byte(0x05);
	IIC_Wait_ACK();
	IIC_WR_Byte(0x05);
	IIC_Wait_ACK();
	IIC_WR_Byte(0x05);
	IIC_Wait_ACK();
	 
	 
	
	IIC_Stop();
	
	
	
	
}

/************
¹¦ÄÜ£ºÏÔÊ¾ÎÂ¶ÈÊý¾Ý£¬Êª¶ÈÊý¾Ý
²ÎÊý£º
		*data    ÎÂÊª¶ÈÊý¾Ý»º³åÇø
		i        Êý×é½çÏÞ
**************/
void PCF8562_Display(uint8_t *data,uint8_t i)
{
	IIC_Start();
	
	//·¢ËÍÃüÁî³õÊ¼»¯
	IIC_WR_Byte(PCF8562);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_1);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_2);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_3);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_5);
	IIC_Wait_ACK();
	
	//·¢ËÍÊý¾Ý
	for(i = 1;i < 7;i++) {
	
		IIC_WR_Byte(data[i]);
		IIC_Wait_ACK();
	}	
}

/**********************************/
void Data_F(int f,uint8_t *buf)
{
	uint16_t Temp1;
	
	if (1)//Sensor.Power_Value > 20
	{
		if(1)//!Sensor.Error
		{
			/**/
			if((f > 30218)||(f < -6700)) //?? 
			{
				buf[6] = SEG_T[10];		//Err
				buf[5] = SEG_T[11];
				buf[4] = SEG_T[11];
			}
			
			//    ??????? X.X??XX.X
			else if(f >9 && f <= 9999)	
			{
				//????,??,??
				buf[6] = SEG_T[f/1000%10];
				buf[5]  = SEG_T[f/100%10]|0x10;		//Dot
				buf[4]  = SEG_T[f/10%10]|0x10;		//??F??
				//????????0,?0??
				if(buf[6] == (SEG_T[0]))//???
				{
					buf[6] = 0x0;
				}
			}
			
			//???????0???,???0??
			else if(f > 0 && f <= 9)
			{
				buf[6] = 0x0;
				buf[5]  = 0x0;		
				buf[4]  = SEG_T[0] |0x10;		//??F??
			}
			
			//??37.7?,?99.9,????????
			else if(f>9999)				
			{
				buf[6] = SEG_T[f/10000];
				buf[5] = SEG_T[f/1000%10];
				buf[4]  = SEG_T[f/100%10]|0x10;;		//??F??					
			}
			
			//??0-X???
			else if(f <= -4 && f >= -9)   // ???????????
			{
				Temp1 = (unsigned int)(0-f);
				buf[6] = SEG_T[12];
				buf[5]  = SEG_T[f/100%10];		
				buf[4]  = SEG_T[Temp1] |0x10;		//??F??	//??F??	
			}
			//??-0.1 ~ -9.9???
			else if(f <= -10 && f >= -999)
			{
				Temp1 = (unsigned int)(0-f);
				//????,??,??
				buf[6] = SEG_T[Temp1/1000%10];
				buf[5]  = SEG_T[Temp1/100%10]|0x10;		//Dot
				buf[4]  = SEG_T[Temp1/10%10]|0x10;		//??F??
				//????????0,?0??
				if(buf[6] == (SEG_T[0]))//??“-”?
				{
					buf[6] = SEG_T[12];
				}
			}
			//??-10.0 ~ -50 
			else if(f < -999 && f >= -5000)         
			{
				Temp1 = (unsigned int)(0-f);
				//????,??,??
				buf[6] = SEG_T[12];
				buf[5]  = SEG_T[Temp1/1000%10];		
				buf[4]  = SEG_T[Temp1/100%10]|0x10;		//??F??
			}		
		}
		
		if( JH_Read_Status() == 0xff)//´«¸ÐÆ÷´íÎó
		{
			buf[6] =0x4f;// E;
			buf[5] =0x42;// r;
			buf[4] =0x42;// r;
		}
		
	}
}
	
void Data_T(int t,uint8_t *buf)
{
	uint16_t Temp1;
	if (1)//Sensor.Power_Value > 20
	{
		if(1)//!Sensor.Error
		{
			if((t >= 1501)||(t < -550)) //?? 550
			{
				buf[6] = SEG_T[10];		//Err
				buf[5] = SEG_T[11];
				buf[4] = SEG_T[11];
			}
			else if(t >= 0)	//0~99.9?
			{
				Temp1 = (unsigned int)t;
				buf[6] = SEG_T[Temp1/100%10]|0x10;
				buf[5]  = SEG_T[Temp1/10%10]|0x10;//Dot
				buf[4]  = SEG_T[Temp1%10];
				if(buf[6] == (SEG_T[0]|0x10))//???
					buf[6] = 0x10;
				
				if(Temp1 > 999)
				{
				buf[6] = SEG_T[Temp1/1000]|0x10;
				buf[5]  = SEG_T[Temp1/100%10];//Dot
				buf[4]  = SEG_T[Temp1/10%100%10];
				if(buf[6] == (SEG_T[0]|0x10))//???
					buf[6] = 0x10;
				 }
			}
			else if((0 > t)&&( t > -100))//-x.x
			{
				Temp1 = (unsigned int)(0-t);
				buf[6] = SEG_T[12]|0x10;		//-
				buf[5]  = SEG_T[Temp1/10%10]|0x10;//Dot
				buf[4]  = SEG_T[Temp1%10];
			}
			else 			//-xx
			{
				Temp1 = (unsigned int)((0-t)/10);  //????,???10
				buf[6] = SEG_T[12]|0x10;//????-
				buf[5]  = SEG_T[Temp1/10%10];//????
				buf[4]  = SEG_T[Temp1%10];//????
			}
		}			
			
		if( JH_Read_Status() == 0xff)//´«¸ÐÆ÷´íÎó
		{
			buf[6] =0x4f;// E;
			buf[5] =0x42;// r;
			buf[4] =0x42;// r;
		}
		


	}
}
	
void Data_RH(int rh,uint8_t *buf)
{
	uint16_t Temp1;
	if (1)//Sensor.Power_Value > 20
	{
		if(1)//!Sensor.Error
		{
			if((rh >= 1000)||(rh <= 0)) //³¬³ö
			{
				buf[1] =SEG_RH[10];// E;
				buf[2] =SEG_RH[11];// r;
				buf[3] =SEG_RH[11];// r;
			}
			else	//0~99.9RH
			{ 
				
				
				Temp1 = (unsigned int)rh;
				
				buf[1] = SEG_RH[Temp1/100%10]|0x01;
				buf[2] = SEG_RH[Temp1/10%10]|0x01;//Dot
				buf[3] = SEG_RH[Temp1%10]|0x01;
				
				if(buf[1] == (SEG_RH[0]|0x01))//ÁãÏûÒþ
					buf[1] = 0x01;
			}
		}
		if( JH_Read_Status() == 0xff)//´«¸ÐÆ÷´íÎó
		{
			buf[1] =SEG_RH[10];// E;
			buf[2] =SEG_RH[11];// r;
			buf[3] =SEG_RH[11];// r;
		}
	}
}

void PCF8562_OFF(void)
{
	IIC_Start();
	
	//·¢ËÍÃüÁî³õÊ¼»¯
	IIC_WR_Byte(PCF8562);
	IIC_Wait_ACK();
	IIC_WR_Byte(0xc0);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_2);
	IIC_Wait_ACK();
	IIC_WR_Byte(COMMOND_3);
	IIC_Wait_ACK();
	IIC_WR_Byte(0x1);
	IIC_Wait_ACK();
	
	IIC_Stop();
}
