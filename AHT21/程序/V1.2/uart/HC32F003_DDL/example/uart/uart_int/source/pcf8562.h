#ifndef _PCF8562_H_
#define _PCF8562_H_

#include "ddl.h"
#include"sensor3123.h"

#define PCF8562    		0x70

#define COMMOND_1			0xc8						//��ʾ��ʽ
#define	COMMOND_2			0xe0						//����ѡ��
#define	COMMOND_3			0xf0						//��˸��ʽ
#define	COMMOND_4			0x0							//���ݵ�ַ
#define	COMMOND_5			0x1							//���ݵ�ַ

void SDA_High(void);
void SDA_Low(void);
void SDA_IN_FLOATING(void);

void SCL_High(void);
void SCL_Low(void);

/***************
IIC�ײ�����
****************/
void IIC_Init(void);
void IIC_Start(void);	
void IIC_WR_Byte(uint8_t Byte);
uint8_t IIC_RD_Byte(void);
uint8_t IIC_Wait_ACK(void);
void IIC_Send_ACK(void)	;
void IIC_Send_NOT_ACK(void);
void IIC_Stop(void);


/***************
PCF8562��������
***************/
void PCF8562_Init(void);
void PCF8562_Display(uint8_t *data,uint8_t i);
void PCF8562_OFF(void);


/**************
������������
***************/
void Data_F(int f,uint8_t *buf);
void Data_T(int t,uint8_t *buf);
void Data_RH(int rh,uint8_t *buf);


#endif
