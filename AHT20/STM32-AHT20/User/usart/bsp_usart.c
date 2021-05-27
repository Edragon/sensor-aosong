#include "bsp_usart.h"


// 接收缓冲，最大100个字节
uint8_t USART_RX_BUF[100];
// 接收状态标记位
uint16_t USART_RX_FLAG=0;


/**************************************************
		配置嵌套向量中断控制器NVIC
**************************************************/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  // 嵌套向量中断控制器组选择
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  // 配置USART为中断源
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  // 抢断优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  // 子优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  // 使能中断
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  // 初始化配置NVIC
  NVIC_Init(&NVIC_InitStructure);
}


/**************************************************
			USART初始化配置
**************************************************/
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	// 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);	
	
	// 使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);
}




/**************************************************
			发送一个字节
**************************************************/
void Usart_SendByte(USART_TypeDef * pUSARTx, uint8_t ch)
{
	// 发送一个字节数据到USART
	USART_SendData(pUSARTx, ch);
	// 等待发送数据寄存器为空
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}




/**************************************************
				发送字符串
**************************************************/
void Usart_SendString(USART_TypeDef * pUSARTx, char *str)
{
	do
	{
		Usart_SendByte(pUSARTx, *str++);
	}while(*str != '\0');
	// 等待发送完成
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);
}




/***************************************************
重定向c库函数printf到串口，重定向后可使用printf函数
****************************************************/
int fputc(int ch, FILE *f)
{
	// 发送一个字节数据到串口
	USART_SendData(DEBUG_USARTx, (uint8_t) ch);
	// 等待发送完毕
	while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
	return (ch);
}


/*********************************************************
重定向c库函数scanf到串口，重定向后可使用scanf、getchar函数
**********************************************************/
int fgetc(FILE *f)
{
	// 等待串口输入数据
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(DEBUG_USARTx);
}



/*********************************************************
    					串口中断函数
**********************************************************/
void DEBUG_USART_IRQHandler(void)
{
	uint8_t temp;
	//接收中断
	if(USART_GetFlagStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET)
	{
		// 读取接收的数据
		temp = USART_ReceiveData(DEBUG_USARTx);
		//接收未完成
		if((USART_RX_FLAG & 0x8000)==0)
		{
			//接收到了0x0d
			if(USART_RX_FLAG & 0x4000)
			{
				// 接收错误,重新开始
				if(temp != 0x0a) USART_RX_FLAG=0;
				// 接收完成
				else USART_RX_FLAG |= 0x8000;
			}
			// 还未接收到0x0d
			else
			{
				if(temp == 0x0d) USART_RX_FLAG |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_FLAG & 0x3FFF]=temp;
					USART_RX_FLAG++;
					//接收数据错误，重新开始接收
					if(USART_RX_FLAG > 99) USART_RX_FLAG=0;
				}
			}
		}
	}
}

