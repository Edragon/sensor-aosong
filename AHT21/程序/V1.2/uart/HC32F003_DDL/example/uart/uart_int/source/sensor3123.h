#ifndef _SENSOR3123_H_
#define _SENSOR3123_H_

#include "ddl.h"

/*SDA����ߵ�ƽ*/
void SDA_Pin_Output_High(void);
/*SDA����͵�ƽ*/
void SDA_Pin_Output_Low(void);
/*����SDAΪ��������*/
void SDA_Pin_IN_FLOATING(void);
/*SCL����ߵ�ƽ*/
void SCL_Pin_Output_High(void);
/*SCL����͵�ƽ*/
void SCL_Pin_Output_Low(void);

/*IIC��ʼ�ź�*/
void I2C_Start(void);	
/*IICдһ���ֽ�*/
void ZSSC_I2C_WR_Byte(uint8_t Byte);
/*IIC��һ���ֽ�*/
uint8_t ZSSC_I2C_RD_Byte(void);
/*IIC���յ�ACK*/
uint8_t Receive_ACK(void);
/*IIC����AKC*/
void Send_ACK(void)	;
/*IIC�����޷���ACK*/
void Send_NOT_ACK(void);
/*IICֹͣ�ź�*/
void Stop_I2C(void);
/*������״̬���ݻ�ȡ*/
void ZSSC3123_UpdateMode_I2C_Data_Fatch(uint8_t *MRdata);
//void JH_Read_OTPdata(u8 addr);

/*������������*/
void JH_Read_CTdata(uint32_t *ct);

/*��������ʼ��*/
void JH_Init(void);
/*����������ACָ��*/
void JH_SendAC(void);
uint8_t JH_Read_Cal_Enable(void);

/*�ػ����Ƶ�ƽ����*/
void ONOFF_Pin_Output_Low(void);
/*�������Ƶ�ƽ����*/
void ONOFF_Pin_Output_High(void);

void SensorDelay_us(uint32_t t);

extern uint8_t JH_Read_Status(void);
#endif
