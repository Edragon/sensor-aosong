#ifndef _BSP_AHT20_H_
#define _BSP_AHT20_H_

#include "stm32f10x.h"  
#include "stm32f10x.h" 
#include "iic.h"
#include "delay.h"



void AHT20_Init(void);					//初始化AHT20
void temphum_init(void);				//初始化温湿度传感器
uint8_t AHT20_Read_Status(void);		//读取AHT20的状态寄存器
void AHT20_SendAC(void);				//向AHT20发送AC命令
void AHT20_Read_CTdata(uint32_t *ct);	//没有CRC校验，直接读取AHT20的温度和湿度数据

#endif

