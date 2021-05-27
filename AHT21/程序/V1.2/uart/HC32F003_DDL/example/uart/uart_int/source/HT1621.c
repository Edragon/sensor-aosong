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

//���϶��л�
/*const unsigned char seg_tab789[]= {
	

};*/

/***********************************************/
//1621���Դ���4λ*32��,����16*8λ�ļĴ�����������Ҫ�������Դ�Ÿ�4λ,ż���ŵ�4λ
/*****************************************************************/

/*
//1621��ʼ������
const unsigned char Init_Tab[10]=
{
	0b00000001,//��ϵͳ��������
	0b00000011,//��LCDƫѹ����������
	0b00000101,//WDT �����־���ʧЧ����
	0b00000110,//ʱ�����ʹ������
	//0b00011000,//ϵͳʱ��ԴƬ��RC��������
	0b00010100,//ϵͳʱ��ԴƬ�ⲿ��������
	0b00101001,//LCD1/3ƫѹѡ��4������������
	//0b01000000,//����Ƶ��4KHz����
	//0b00001001,//�������������
	0b10100000,//ʱ��/WDT ʱ�����1Hz����
	0b10001000 //ʹ/IRQ �����Ч����
};*/


void SET_ALL_PINS_OUTPP(void)
{
    /**********LCD_CS***********/
//	LCD_GPIO_nCS->ODR |= LCD_nCS;			//���'1'
//	LCD_GPIO_nCS->DDR |= LCD_nCS;			//���ģʽ
//	LCD_GPIO_nCS->CR1 |= LCD_nCS;			//����ģʽ
//	LCD_GPIO_nCS->CR2 |= LCD_nCS;			//�������
	
	Gpio_InitIOExt(2,5,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P15 to output
	Gpio_SetIO(2,5,1); 

    /**********LCD_WR***********/
//	LCD_GPIO_WR->ODR  |= LCD_WR;			//���'1'
//	LCD_GPIO_WR->DDR  |= LCD_WR;			//���ģʽ
//	LCD_GPIO_WR->CR1  |= LCD_WR;			//����ģʽ
//	LCD_GPIO_WR->CR2  |= LCD_WR;			//�������
	Gpio_InitIOExt(3,4,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P15 to output
	Gpio_SetIO(3,4,1); 

    /*********LCD_DATA**********/
//	LCD_GPIO_SDA->ODR |= LCD_SDA;			//���'1'
//	LCD_GPIO_SDA->DDR |= LCD_SDA;			//���ģʽ
//	LCD_GPIO_SDA->CR1 |= LCD_SDA;			//����ģʽ
//	LCD_GPIO_SDA->CR2 |= LCD_SDA;			//�������
  Gpio_InitIOExt(2,6,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P15 to output
	Gpio_SetIO(2,6,1); 
}

void SET_ALL_PINS_INFL(void)
{
    /**********LCD_CS***********/
//	LCD_GPIO_nCS->DDR &= ~(LCD_nCS);			//����ģʽ
//	LCD_GPIO_nCS->CR1 &= ~(LCD_nCS);			//����ģʽ
//	LCD_GPIO_nCS->CR2 &= ~(LCD_nCS);			//���ж�

//    /**********LCD_WR***********/
//	LCD_GPIO_WR->DDR &= ~(LCD_WR);			//����ģʽ
//	LCD_GPIO_WR->CR1 &= ~(LCD_WR);			//����ģʽ
//	LCD_GPIO_WR->CR2 &= ~(LCD_WR);			//���ж�

//    /*********LCD_DATA**********/
//	LCD_GPIO_SDA->DDR &= ~(LCD_SDA);			//����ģʽ
//	LCD_GPIO_SDA->CR1 &= ~(LCD_SDA);			//����ģʽ
//	LCD_GPIO_SDA->CR2 &= ~(LCD_SDA);			//���ж�
	
	Gpio_InitIO(2, 5, GpioDirIn);
	Gpio_InitIO(3, 4, GpioDirIn);
	Gpio_InitIO(2, 6, GpioDirIn);
}

void Init_LCDPORT(void)
{
	SET_ALL_PINS_OUTPP();
}

//��ʱt��΢��
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
	//����SDA,CS��SCL
	SET_ALL_PINS_OUTPP();
	CLR_nCS;
	Send_Data(0x80,3);//��3λ����ģʽ����100 0b10000000
	//��9λ������� 
	Send_Data(0x01,9);//0b00000001,//��ϵͳ��������
	Send_Data(0x03,9);//0b00000011,//��LCDƫѹ����������
	Send_Data(0x05,9);//0b00000101,//WDT �����־���ʧЧ��
	Send_Data(0x06,9);//0b00000110,//ʱ�����ʹ������
	//Send_Data(0x14,9);//0b00010100,//ϵͳʱ��ԴƬ�ⲿ��������
	Send_Data(0x18,9);//0b00011000,//ϵͳʱ��ԴƬ��RC��������
	Send_Data(0x29,9);//0b00101001,//LCD1/3ƫѹѡ��4������������
	Send_Data(0xa0,9);//0b10100000,//ʱ��/WDT ʱ�����1Hz����
	Send_Data(0x88,9);//0b10001000 //ʹ/IRQ �����Ч����
	
	//0b00011000,//ϵͳʱ��ԴƬ��RC��������
	//0b01000000,//����Ƶ��4KHz����
	//0b00001001,//�������������
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
}


void HT1621_LCD_ON(void)
{
 SET_ALL_PINS_OUTPP();
	CLR_nCS;
	Send_Data(0x80,3);//��3λ����ģʽ����100 0b10000000
	
	Send_Data(0x01,9);//0b00000001,//��ϵͳ��������
	Send_Data(0x03,9);//0b00000011,//��LCDƫѹ����������
	SET_nCS;
	SET_WR;
	SET_ALL_PINS_INFL();
 }	

 
 void HT1621_LCD_OFF(void)
{
 SET_ALL_PINS_OUTPP();
	CLR_nCS;
	Send_Data(0x80,3);//��3λ����ģʽ����100 0b10000000
	
	Send_Data(0x00,9);//0b00000001,//��ϵͳ��������
	Send_Data(0x02,9);//0b00000011,//��LCDƫѹ����������
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
//extern uint8_t  gFlashCount; 	   //�˵���˸���Ƽ���
void Init_LCD(void)
{
	Init_LCDPORT();
	Init_HT1621();

	HT1621.Start = 0;//������ʾID 2S

	HT1621.CelSignEnable = 1;				//�����϶�ͼ��
	HT1621.DisBuf[4] |= 0x01;//����
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
			if((f > 30218)||(f < -6700)) //���� 
			{
				HT1621.DisBuf[10] = seg_tab123[0x0E];		//Err
				HT1621.DisBuf[9]  = seg_tab123[0x10];
				HT1621.DisBuf[8]  = seg_tab123[0x10];
			}
			
			//    ������ʾ���϶� X.X����XX.X
			else if(f >9 && f <= 9999)	
			{
				//��ȡǧλ����λ��ʮλ
				HT1621.DisBuf[10] = seg_tab123[f/1000%10];
				HT1621.DisBuf[9]  = seg_tab123[f/100%10]|0x10;		//Dot
				HT1621.DisBuf[8]  = seg_tab123[f/10%10]|0x10;		//��ʾF��־
				//����һ�������Ϊ0����0ȥ��
				if(HT1621.DisBuf[10] == (seg_tab123[0]))//������
				{
					HT1621.DisBuf[10] = 0x0;
				}
			}
			
			//�жϻ��϶Ƚӽ�0��ʱ��һ�ɰ�0����
			else if(f > 0 && f <= 9)
			{
				HT1621.DisBuf[10] = 0x0;
				HT1621.DisBuf[9]  = 0x0;		
				HT1621.DisBuf[8]  = seg_tab123[0] |0x10;		//��ʾF��־
			}
			
			//����37.7�ȣ���99.9��������ʾС������
			else if(f>9999)				
			{
				HT1621.DisBuf[10] = seg_tab123[f/10000];
				HT1621.DisBuf[9] = seg_tab123[f/1000%10];
				HT1621.DisBuf[8]  = seg_tab123[f/100%10]|0x10;;		//��ʾF��־					
			}
			
			//��ʾ0-X������
			else if(f <= -4 && f >= -9)   // ��ʾ��������µĻ��϶�
			{
				Temp1 = (unsigned int)(0-f);
				HT1621.DisBuf[10] = seg_tab123[18];
				HT1621.DisBuf[9]  = seg_tab123[f/100%10];		
				HT1621.DisBuf[8]  = seg_tab123[Temp1] |0x10;		//��ʾF��־	//��ʾF��־	
			}
			//��ʾ-0.1 ~ -9.9������
			else if(f <= -10 && f >= -999)
			{
				Temp1 = (unsigned int)(0-f);
				//��ȡǧλ����λ��ʮλ
				HT1621.DisBuf[10] = seg_tab123[Temp1/1000%10];
				HT1621.DisBuf[9]  = seg_tab123[Temp1/100%10]|0x10;		//Dot
				HT1621.DisBuf[8]  = seg_tab123[Temp1/10%10]|0x10;		//��ʾF��־
				//����һ�������Ϊ0����0ȥ��
				if(HT1621.DisBuf[10] == (seg_tab123[0]))//��ʾ��-����
				{
					HT1621.DisBuf[10] = seg_tab123[18];
				}
			}
			//��ʾ-10.0 ~ -50 
			else if(f < -999 && f >= -5000)         
			{
				Temp1 = (unsigned int)(0-f);
				//��ȡǧλ����λ��ʮλ
				HT1621.DisBuf[10] = seg_tab123[18];
				HT1621.DisBuf[9]  = seg_tab123[Temp1/1000%10];		
				HT1621.DisBuf[8]  = seg_tab123[Temp1/100%10]|0x10;		//��ʾF��־
			}		
		}
		
		else//����������
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
			if((t >= 1501)||(t < -550)) //���� 550
			{
				HT1621.DisBuf[10] = seg_tab123[0x0E];		//Err
				HT1621.DisBuf[9]  = seg_tab123[0x10];
				HT1621.DisBuf[8]  = seg_tab123[0x10];
			}
			else if(t >= 0)	//0~99.9��
			{
				Temp1 = (unsigned int)t;
				HT1621.DisBuf[10] = seg_tab123[Temp1/100%10]|0x10;
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10]|0x10;//Dot
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];
				if(HT1621.DisBuf[10] == (seg_tab123[0]|0x10))//������
					HT1621.DisBuf[10] = 0x10;
				
				if(Temp1>999)
				{
				 HT1621.DisBuf[10] = seg_tab123[Temp1/1000]|0x10;;
				 HT1621.DisBuf[9]  = seg_tab123[Temp1/100%10];//Dot
				 HT1621.DisBuf[8]  = seg_tab123[Temp1/10%100%10];
				 if(HT1621.DisBuf[10] == (seg_tab123[0]|0x10))//������
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
				Temp1 = (unsigned int)((0-t)/10);  //תΪ������������10
				HT1621.DisBuf[10] = seg_tab123[18]|0x10;//��ʾ����-
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10];//��ʾʮλ
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];//��ʾ��λ
			}
		}			
			
		else//����������
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
			if((rh >= 1000)||(rh <= 0)) //����
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
				
				if(HT1621.DisBuf[5] == (seg_tab456[0]|0x01))//������
					HT1621.DisBuf[5] = 0x01;
			}
		}
		else//����������
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
			//����1
			/**/
			if((Val1 >= 1501)||(Val1 < -550)) //���� 550
			{
				HT1621.DisBuf[10] = seg_tab123[0x0E];		//Err
				HT1621.DisBuf[9]  = seg_tab123[0x10];
				HT1621.DisBuf[8]  = seg_tab123[0x10];
			}
			else if(Val1 >= 0)	//0~99.9��
			{
				Temp1 = (unsigned int)Val1;
				HT1621.DisBuf[10] = seg_tab456[Temp1/100%10]|0x10;
				HT1621.DisBuf[9]  = seg_tab456[Temp1/10%10]|0x10;//Dot
				HT1621.DisBuf[8]  = seg_tab456[Temp1%10];
				if(HT1621.DisBuf[10] == (seg_tab456[0]|0x10))//������
					HT1621.DisBuf[10] = 0x10;
				
				if(Temp1>999)
				{
				 HT1621.DisBuf[10] = seg_tab123[Temp1/1000]|0x10;;
				 HT1621.DisBuf[9]  = seg_tab123[Temp1/100%10];//Dot
				 HT1621.DisBuf[8]  = seg_tab123[Temp1/10%100%10];
				 if(HT1621.DisBuf[10] == (seg_tab123[0]|0x10))//������
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
				Temp1 = (unsigned int)((0-Val1)/10);  //תΪ������������10
				HT1621.DisBuf[10] = seg_tab123[18]|0x10;//��ʾ����-
				HT1621.DisBuf[9]  = seg_tab123[Temp1/10%10];//��ʾʮλ
				HT1621.DisBuf[8]  = seg_tab123[Temp1%10];//��ʾ��λ
			}  
			
			//����2
			if((Val2 >= 1000)||(Val2 <= 0)) //����
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
				
				if(HT1621.DisBuf[5] == (seg_tab456[0]|0x01))//������
					HT1621.DisBuf[5] = 0x01;
			}
		}
		else//����������
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
	Send_Data(0xa0,3);//��3λ��д ��ģʽ�������101
	Send_Data(0,6);//��6λ�׵�ַ00h
	for(t=0;t<=10;t++)
	{
		seg_temp =HT1621.DisBuf[t];
		Send_Data(seg_temp,8);//���Դ�������		
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
	Send_Data(0xa0,3);//��3λ��д ��ģʽ�������101
	Send_Data(0x9<<2,6);//��6λ�׵�ַ00h
	
	seg_temp =HT1621.DisBuf[1];
	Send_Data(0x1f,4);//���Դ�������		
	
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
	Send_Data(0xa0,3);//��3λ��д ��ģʽ�������101
	Send_Data(0x0,6);//��6λ�׵�ַ00h
	
	seg_temp =HT1621.DisBuf[0];
	Send_Data(0x00,128);//���Դ�������		
	
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
//	Send_Data(0xa0,3);//��3λ��д ��ģʽ�������101
//	Send_Data(0,6);//��6λ�׵�ַ00h
//	for(t=0;t<=10;t++)
//	{
//		seg_temp = 0;
//		Send_Data(seg_temp,8);//���Դ�������		
//	}
//	SET_nCS;
//	SET_WR;
//	SET_ALL_PINS_INFL();
//}


