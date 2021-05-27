#include"sensor3123.h"
//#include"delay.h"
//#include "main.h"
#include "gpio.h"
#include "wdt.h"
extern void send_Byte(uint8_t temp);


//端口配置为数字通用端口
//a) 设置寄存器 Px_ADS 为 0
//b) 设置寄存器 Px_sel 为 0
//c) 设置寄存器 PxDIR 为 1：端口方向为输入， CPU 可以读取端口的状态 PxIN
//d) 设置寄存器 PxDIR 为 0：端口方向为输出
//e) 设置寄存器 PxOUT 为 1：端口输出高电平
//f) 设置寄存器 PxOUT 为 0：端口输出低电平

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

void Delay_3us(void)		//当STM8 CPU_CLK=8M 时延时2微秒
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}

void Delay_1us(void)		//当STM8 CPU_CLK=8M 时延时2微秒
{	
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
	Delay_N10us(1);
}

/*SDA输出高电平*/
void SDA_Pin_Output_High(void)
{
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P15 to output
	Gpio_SetIO(1,5,1); 
}

/*关机控制电平函数*/
void ONOFF_Pin_Output_Low(void)
{
	//Gpio_InitIOExt(3,3,GpioDirOut,TRUE, FALSE, FALSE, FALSE);//config P33 to output
	Gpio_InitIOExt(3,3,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(3,3,0); 
}

/*开机控制电平函数*/
void ONOFF_Pin_Output_High(void)
{
	Gpio_InitIOExt(3,3,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P33 to output
	Gpio_SetIO(3,3,1); 
}

/*SDA输出低电平*/
void SDA_Pin_Output_Low(void)
{
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);//config P15 to output
	Gpio_SetIO(1,5,0);  
}

/*设置SDA为悬空输入*/
void SDA_Pin_IN_FLOATING(void)
{
	Gpio_InitIO(1, 5, GpioDirIn);
}

/*SCL输出高电平*/
void SCL_Pin_Output_High(void)
{
	Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1, 4, 1);
}

/*SCL输出低电平*/
void SCL_Pin_Output_Low(void)
{
	Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1, 4, 0);
}

//IIC IO初始化
void Init_I2C_Sensor_Port(void)
{	
	Gpio_InitIOExt(1,5,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,5,1);
	
  Gpio_InitIOExt(1,4,GpioDirOut,TRUE, FALSE, TRUE, FALSE);
	Gpio_SetIO(1,4,1);
}

/*IIC开始信号*/
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

/*IIC写一个字节*/
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

/*IIC读一个字节*/
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

/*IIC接收到ACK*/
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

/*IIC发送AKC*/
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

/*IIC发送无返回ACK*/
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

/*IIC停止信号*/
void Stop_I2C(void)	
{
	SDA_Pin_Output_Low();
	SensorDelay_us(8);
	SCL_Pin_Output_High();	
	SensorDelay_us(8);
	SDA_Pin_Output_High();
	SensorDelay_us(8);
}

/*传感器状态数据获取*/
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

/*读传感器数据*/
void JH_Read_CTdata(uint32_t *ct)
{
	//定义6个字节用来获取传感器温度，湿度数据
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
	
	//获取传感器状态是否为0X80
	while(((JH_Read_Status()&0x80)==0x80))
	{
		SensorDelay_us(1508);
		if(cnt++>=100)
		{
		 break;
		 }		
	}
	
	//开始IIC
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

	//湿度数据，共20BIT
	RetuData = (RetuData|Byte_2th)<<8;
	RetuData = (RetuData|Byte_3th)<<8;
	RetuData = (RetuData|Byte_4th);
	//去掉低位的4BIT温度数据
	RetuData =RetuData >>4;
	ct[0] = RetuData;
	
	//温度数据，共20BIT
	RetuData = 0;
	RetuData = (RetuData|Byte_4th)<<8;
	RetuData = (RetuData|Byte_5th)<<8;
	RetuData = (RetuData|Byte_6th);
	//去掉高位的4BIT湿度数据
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

/*传感器初始化*/
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

/*传感器发送AC命令*/
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



