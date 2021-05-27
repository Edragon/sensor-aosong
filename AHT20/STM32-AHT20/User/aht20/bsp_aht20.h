#ifndef _BSP_AHT20_H_
#define _BSP_AHT20_H_

#include "stm32f10x.h"  
#include "stm32f10x.h" 
#include "iic.h"
#include "delay.h"



void AHT20_Init(void);					//��ʼ��AHT20
void temphum_init(void);				//��ʼ����ʪ�ȴ�����
uint8_t AHT20_Read_Status(void);		//��ȡAHT20��״̬�Ĵ���
void AHT20_SendAC(void);				//��AHT20����AC����
void AHT20_Read_CTdata(uint32_t *ct);	//û��CRCУ�飬ֱ�Ӷ�ȡAHT20���¶Ⱥ�ʪ������

#endif

