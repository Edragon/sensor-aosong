#ifndef _SENSOR3123_H_
#define _SENSOR3123_H_

#include "ddl.h"

/*SDA输出高电平*/
void SDA_Pin_Output_High(void);
/*SDA输出低电平*/
void SDA_Pin_Output_Low(void);
/*设置SDA为悬空输入*/
void SDA_Pin_IN_FLOATING(void);
/*SCL输出高电平*/
void SCL_Pin_Output_High(void);
/*SCL输出低电平*/
void SCL_Pin_Output_Low(void);

/*IIC开始信号*/
void I2C_Start(void);	
/*IIC写一个字节*/
void ZSSC_I2C_WR_Byte(uint8_t Byte);
/*IIC读一个字节*/
uint8_t ZSSC_I2C_RD_Byte(void);
/*IIC接收到ACK*/
uint8_t Receive_ACK(void);
/*IIC发送AKC*/
void Send_ACK(void)	;
/*IIC发送无返回ACK*/
void Send_NOT_ACK(void);
/*IIC停止信号*/
void Stop_I2C(void);
/*传感器状态数据获取*/
void ZSSC3123_UpdateMode_I2C_Data_Fatch(uint8_t *MRdata);
//void JH_Read_OTPdata(u8 addr);

/*读传感器数据*/
void JH_Read_CTdata(uint32_t *ct);

/*传感器初始化*/
void JH_Init(void);
/*传感器发送AC指令*/
void JH_SendAC(void);
uint8_t JH_Read_Cal_Enable(void);

/*关机控制电平函数*/
void ONOFF_Pin_Output_Low(void);
/*开机控制电平函数*/
void ONOFF_Pin_Output_High(void);

void SensorDelay_us(uint32_t t);

extern uint8_t JH_Read_Status(void);
#endif
