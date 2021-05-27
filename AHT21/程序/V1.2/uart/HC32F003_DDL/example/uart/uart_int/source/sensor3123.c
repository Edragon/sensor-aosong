#include"sensor3123.h"
//#include"delay.h"
//#include "main.h"
#include "gpio.h"
#include "wdt.h"
extern void send_Byte(uint8_t temp);


//�˿�����Ϊ����ͨ�ö˿�
//a) ���üĴ��� Px_ADS Ϊ 0
//b) ���üĴ��� Px_sel Ϊ 0
//c) ���üĴ��� PxDIR Ϊ 1���˿ڷ���Ϊ���룬 CPU ���Զ�ȡ�˿ڵ�״̬ PxIN
//d) ���üĴ��� PxDIR Ϊ 0���˿ڷ���Ϊ���
//e) ���üĴ��� PxOUT Ϊ 1���˿�����ߵ�ƽ
//f) ���üĴ��� PxOUT Ϊ 0���˿�����͵�ƽ

#define P1_ADS_REG     *((volatile uint32_t *)(0x40020D4C))


void Delay_N10us(uint32_t t)
{
  uint32_t k;

   while(t--)
  {
    for (k = 0; k < 2; k++)//110
      ;
  }
}

void SensorDelay_us(uint32_t t)
{
	t = t-2;	
	for(; t>0; t--)
	{
		Delay_N10us(1);
	}

}

void Delay_3us(void)		//��STM8 CPU_CLK=8M ʱ��ʱ2΢��
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}

void Delay_1us(void)		//��STM8 CPU_CLK=8M ʱ��ʱ2΢��
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}

/*SDA����ߵ�ƽ*/
void SDA_Pin_Output_High(void)
{
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P15 to output
	Gpio_SetIO(1,5,1); 
}

/*�ػ����Ƶ�ƽ����*/
void ONOFF_Pin_Output_Low(void)
{
	//Gpio_InitIOExt(3,3,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P33 to output
	Gpio_InitIOExt(3,3,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(3,3,0); 
}

/*�������Ƶ�ƽ����*/
void ONOFF_Pin_Output_High(void)
{
	Gpio_InitIOExt(3,3,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P33 to output
	Gpio_SetIO(3,3,1); 
}

/*SDA����͵�ƽ*/
void SDA_Pin_Output_Low(void)
{
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P15 to output
	Gpio_SetIO(1,5,0);  
}

/*����SDAΪ��������*/
void SDA_Pin_IN_FLOATING(void)
{
	Gpio_InitIO(1, 5, GpioDirIn);
}

/*SCL����ߵ�ƽ*/
void SCL_Pin_Output_High(void)
{
	Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1, 4, 1);
}

/*SCL����͵�ƽ*/
void SCL_Pin_Output_Low(void)
{
	Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1, 4, 0);
}

//IIC IO��ʼ��
void Init_I2C_Sensor_Port(void)
{	
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,5,1);
	
  Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,4,1);
}

/*IIC��ʼ�ź�*/
void I2C_Start(void)		
{
	SDA_Pin_Output_High();
	SensorDelay_us(8);
	SCL_Pin_Output_High();
	SensorDelay_us(8);
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_Low();
	SensorDelay_us(8);   
}

/*IICдһ���ֽ�*/
void ZSSC_I2C_WR_Byte(uint8_t Byte)
{
	uint8_t Data,N,i;	
	Data=Byte;
	i = 0x80;
	for(N=0;N<8;N++)
	{
		SCL_Pin_Output_Low();
		//Delay_us(8);   
		Delay_3us();	
		//i &= Data;
		if(i&Data)
		{
			SDA_Pin_Output_High();
		}
		else
		{
			SDA_Pin_Output_Low();
		}	
		//Delay_3us();
		//Delay_3us();
		//Delay_us(8);	
    SCL_Pin_Output_High();
		Delay_3us();
		//Delay_N10us(1);
		Data <<= 1;
		//Delay_us(5);   
	}
	SCL_Pin_Output_Low();
	SensorDelay_us(8);   
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
}	

/*IIC��һ���ֽ�*/
uint8_t ZSSC_I2C_RD_Byte(void)
{
	uint8_t Byte,i,a;
	Byte = 0;
	SCL_Pin_Output_Low();
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
	for(i=0;i<8;i++)
	{
    SCL_Pin_Output_High();		
		//Delay_us(8);
		//Delay_1us();
		Delay_1us();
		a=0;
		if(Gpio_GetIO(1,5))a=1;
		Byte = (Byte<<1)|a;
		SCL_Pin_Output_Low();
		//Delay_1us();
		Delay_1us();
		//Delay_us(8);
		//Delay_3us();
		//Delay_N10us(1);
	}
  SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
	return Byte;
}

/*IIC���յ�ACK*/
uint8_t Receive_ACK(void)
{
	uint16_t CNT;
	CNT = 0;
	SCL_Pin_Output_Low();	
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);	
	SCL_Pin_Output_High();	
	SensorDelay_us(8);	
	while((Gpio_GetIO(1,5))  && CNT < 100) 
	CNT++;
	if(CNT == 100)
	{
		//Read_Error = I2C_Error;
		return 0;
	}
 	SCL_Pin_Output_Low();	
	SensorDelay_us(8);	
	return 1;
}

/*IIC����AKC*/
void Send_ACK(void)		
{
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);	
	SDA_Pin_Output_Low();
	SensorDelay_us(8);	
	SCL_Pin_Output_High();	
	SensorDelay_us(8);
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);
	SDA_Pin_IN_FLOATING();
	SensorDelay_us(8);
}

/*IIC�����޷���ACK*/
void Send_NOT_ACK(void)	
{
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
	SCL_Pin_Output_High();	
	SensorDelay_us(8);		
	SCL_Pin_Output_Low();	
	SensorDelay_us(8);
  SDA_Pin_Output_Low();
	SensorDelay_us(8);
}

/*IICֹͣ�ź�*/
void Stop_I2C(void)	
{
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_High();	
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
}

/*������״̬���ݻ�ȡ*/
uint8_t JH_Read_Status(void)
{

	uint8_t Byte_first;	
	I2C_Start();
	ZSSC_I2C_WR_Byte(0x71);
	Receive_ACK();
	Byte_first = ZSSC_I2C_RD_Byte();
		
/*	Send_ACK();
	Byte_second = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_third = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_fourth = ZSSC_I2C_RD_Byte();*/
	Send_NOT_ACK();
	Stop_I2C();
	/*	I2C_Start();
		I2C_WR_Byte(0x51);
		Receive_ACK();
		I2C_RD_Byte();
		Send_NOT_ACK();
		Stop_I2C();*/

	return Byte_first;
}

extern uint16_t aht10_read_tick;

/*������������*/
void JH_Read_CTdata(uint32_t *ct)
{
	//����6���ֽ�������ȡ�������¶ȣ�ʪ������
	volatile uint8_t  Byte_1th=0;
	volatile uint8_t  Byte_2th=0;
	volatile uint8_t  Byte_3th=0;
	volatile uint8_t  Byte_4th=0;
	volatile uint8_t  Byte_5th=0;
	volatile uint8_t  Byte_6th=0;
	
	uint32_t RetuData = 0;
	uint16_t cnt = 0;

	JH_SendAC();

	aht10_read_tick = 0;
	
	while(aht10_read_tick<250);

	cnt = 0;
	
	//��ȡ������״̬�Ƿ�Ϊ0X80
	while(((JH_Read_Status()&0x80)==0x80))
	{
		SensorDelay_us(1508);
		if(cnt++>=100)
		{
		 break;
		 }		
	}
	
	//��ʼIIC
	I2C_Start();

	ZSSC_I2C_WR_Byte(0x71);
	Receive_ACK();
	
	Byte_1th = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_2th = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_3th = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_4th = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_5th = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_6th = ZSSC_I2C_RD_Byte();
	Send_NOT_ACK();
	
	Stop_I2C();

	//ʪ�����ݣ���20BIT
	RetuData = (RetuData|Byte_2th)<<8;
	RetuData = (RetuData|Byte_3th)<<8;
	RetuData = (RetuData|Byte_4th);
	//ȥ����λ��4BIT�¶�����
	RetuData =RetuData >>4;
	ct[0] = RetuData;
	
	//�¶����ݣ���20BIT
	RetuData = 0;
	RetuData = (RetuData|Byte_4th)<<8;
	RetuData = (RetuData|Byte_5th)<<8;
	RetuData = (RetuData|Byte_6th);
	//ȥ����λ��4BITʪ������
	RetuData = RetuData&0xfffff;
	ct[1] =RetuData; 
}


void JH_Read_OTPdata(uint8_t addr)
{
	uint8_t Byte_first,Byte_second,Byte_third;
	
	I2C_Start();
	
	ZSSC_I2C_WR_Byte(0x70);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(addr);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	Stop_I2C();
	SensorDelay_us(58);

	SensorDelay_us(28);
	I2C_Start();
	ZSSC_I2C_WR_Byte(0x71);
	Receive_ACK();
	Byte_first = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_second = ZSSC_I2C_RD_Byte();
	Send_ACK();
	Byte_third = ZSSC_I2C_RD_Byte();
	Send_NOT_ACK();
	Stop_I2C();
}

/*��������ʼ��*/
void JH_Init(void)
{
	Init_I2C_Sensor_Port();
	
	I2C_Start();
	
	ZSSC_I2C_WR_Byte(0x70);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0xa8);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	
	Stop_I2C();

	SensorDelay_us(11038);

	I2C_Start();
	
	ZSSC_I2C_WR_Byte(0x70);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0xe1);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x08);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	
	Stop_I2C();	
}

/*����������AC����*/
void JH_SendAC(void)
{
	I2C_Start();
	
	ZSSC_I2C_WR_Byte(0x70);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0xac);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x30);
	Receive_ACK();
	ZSSC_I2C_WR_Byte(0x00);
	Receive_ACK();
	
	Stop_I2C();
}

uint8_t JH_Read_Cal_Enable(void)
{
	uint8_t val = 0;//ret = 0,
 
  val = JH_Read_Status();
	 if((val & 0x68)==0x08)
		 return 1;
   else  return 0;
 }



