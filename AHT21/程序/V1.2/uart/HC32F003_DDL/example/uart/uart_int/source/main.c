/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** Function:
 **						1、长按开关机、短按实现温度与华氏度的转化
 ** 					2、LED屏显示数据，串口并打印
 ** 					3、串口发送命令，获取当前温/华氏度湿度、设置误差校准、是否串口自动输出
 **								COMMOND：
 **   										1、get
 **												2、set t=x.x,h=x.x
 **												3、auto=0(关闭)/auto=1(开启) 
 **   
 ** Add Function:
 **								1、新增USB检测判断	
 **								2、新增电池供电模式，USB供电模式
 **								3、关机改为长按不用松手即可
 ** Version:V1.1
 ** Author :wujinpeng by 2020-4-26
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "ddl.h"
#include "uart.h"
#include "bt.h"
#include "lpm.h"
#include "gpio.h"
#include "sensor3123.h"
//#include "HT1621.h"
#include "flash.h"
#include "wdt.h"

#include "pcf8562.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                             
 ******************************************************************************/
 
#define     T1_PORT                 (3)
#define     T1_PIN                  (3)


#define START_MODE 0
#define NORMAL_MODE 1
#define VERIFY_MODE 2
#define PREPARE_MODE 3

#define BUF_NUM 10
/*define com cmd*/
#define ENTER_VERIFY_MODE 0x10
#define EXIT_VERIFY_MODE  0x20
#define VERIFY_TEMP 0x30
#define VERIFY_HUMI 0x31
#define READ_TEMP 0x40
#define READ_HUMI 0x41

#define DP_WAIT_MODE 0
#define DP_NORMAL_MODE 1

#define PortReceiveBufferMAX  20	 //接收缓冲区长度

#define RCH_CR_TRIM_8M_VAL          (*((volatile uint16_t*) (0x00100C06ul)))



uint8_t PortReceiveBuffer[PortReceiveBufferMAX];	  //接收缓冲区
uint8_t PortWriteBufferPointer=0;	 //写指示
uint8_t PortReadBufferPointer=0;	 //读指示

uint16_t flash_ok = 0;
uint16_t c_add = 0;
uint16_t c_dec = 0;
uint16_t t_add = 0;
uint16_t t_dec = 0;

int c_verify = 0;
int t_verify = 0;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
//uint8_t u8TxData[10] = {0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};//"HUADA MCU!";
//uint8_t u8TxData[10] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
//uint32_t u32RxData[10];
uint8_t u8TxCnt=0,u8RxCnt=0;
//串口接收数据标志
uint8_t rx_flag = 0;


//struct st_com_data {
//uint8_t rx_flag;    //收到下行数据包

//uint8_t rx_mode_cmd;   //接到的命令
//uint8_t u8Data[10];
//};
//struct st_com_data com_data;

/*************************************************************************************************/
	static uint8_t pcf_data[7];
	
	uint8_t lock = 0;
	//长短按标志
	uint8_t key_long_press = 0;
	uint8_t key_short_press = 0;

	//正常开机后标志
	uint8_t init_state = 0;
	//温度与华氏度来回切换标志，短按触发  0默认温度表示
	uint8_t cf_change = 0;
	//温度，湿度传感器结果缓冲区，未转化结果
	uint32_t CT_data[2];
	//定义接收温度，湿度变量
	int c1;
	int t1;
	int f1;
	
//接收到上位机的校准温湿度值
	uint8_t update_flag  = 0;
	//接收串口返回数据
	uint8_t temp_data[20];
	//校准数据
	uint8_t update_t;
	uint8_t update_rh;
	
	uint8_t get_ht_flag = 0;
	//串口自动接收标志，默认为1自动接收开启
	uint8_t auto_flag = 1;
	//用来存放ASSIC与十六进制转换数据
	uint8_t BCDBUF[13];
	//看门口变量
	uint8_t wd_count = 0;
	
	//看门狗定时变量，默认3.28sS
	/*************
	*0x00:1.6ms			0x09:820ms
	*0x01:3.2ms			0x0a:1.64s
	*0x02:6.4ms			0x0b:3.28s
	*0x03:13ms			0x0c:6.55s
	*0x04:26ms			0x0d:13.1s
	*0x05:51ms			0x0e:26.2s
	*0x06:102ms			0x0f:52.4s
	*0x07:205ms
	*0x08:500ms
	********/
	uint8_t wdt_wov = 0x0b;  //3.28s
/************************************************************************************************/

uint8_t operater_mode;  //保存现在的模式状态
uint16_t delay_count = 0;
uint16_t count = 0;
uint16_t u16timer_tick = 0;
uint16_t u16display_tick = 0;
uint16_t aht10_read_tick = 0;
uint32_t address = 0x3e00;
static volatile uint32_t u32FlashTestFlag   = 0;

void TxIntCallback(void)
{

//    if(u8TxCnt<10)
//    {
//        M0P_UART1->SBUF = u32RxData[u8TxCnt];
//        u8TxCnt++;
//    }
//    else //if(u8TxCnt>10)
//    {
//        u8TxCnt = 0;
//        Uart_ClrStatus(UARTCH1,UartTxEmpty);   
//        Uart_DisableIrq(UARTCH1,UartTxIrq);
//        Uart_EnableIrq(UARTCH1,UartRxIrq);
//    }
    
}
void RxIntCallback(void)
{
		
//    u32RxData[u8RxCnt]=Uart_ReceiveData(UARTCH1);
//    u8RxCnt++;
//    if(u8RxCnt>=10)
//    {
//        Uart_DisableIrq(UARTCH1,UartRxIrq);
//        Uart_EnableIrq(UARTCH1,UartTxIrq);
//        u8RxCnt = 0;
//        M0P_UART1->SBUF = u32RxData[0];
//        u8TxCnt = 1;
//    }
	PortReceiveBuffer[PortWriteBufferPointer] = Uart_ReceiveData(UARTCH1);
	PortWriteBufferPointer++;
	if(PortWriteBufferPointer>=PortReceiveBufferMAX)
	{
		PortWriteBufferPointer = 0;
	}
	
	rx_flag = 1;
}
void ErrIntCallback(void)
{
  
}



/************************************************************************************************/
/*******************************************************************************
 * 定时器中断服务函数
 ******************************************************************************/
void Bt2Int(void)
{
    if (TRUE == Bt_GetIntFlag(TIM2))
    {
      Bt_ClearIntFlag(TIM2);
      
			u16timer_tick++;
			aht10_read_tick++;
			count++;
			delay_count++;
    }
}

/*******************************************************************************
 * BT TM2选择模式2 系统时钟为4M  PCLK和HCLK不分频，PRS预除频为1。 2MS进入一个定时器中断
 ******************************************************************************/
en_result_t BtTimerInit(void)
{
    stc_bt_config_t   stcConfig;
    en_result_t       enResult = Error;
    uint16_t          u16ArrData = 0xE0BF;
    uint16_t          u16InitCntData = 0xE0BF;
    
    EnableNvic(TIM2_IRQn, 3, TRUE);
    Bt_EnableIrq(TIM2);
    
    stcConfig.pfnTim2Cb = Bt2Int;
    //P25???????IO
    //Gpio_SetFunc_GATE1_P25();
        
    stcConfig.enGateP = BtPositive;       //门控电平
    stcConfig.enGate  = BtGateDisable;    //门控禁止
    stcConfig.enPRS   = BtPCLKDiv1; //PRS预除频
    stcConfig.enTog   = BtTogDisable; //TOG输出
    stcConfig.enCT    = BtTimer; //定时器还是计数器方式选择
    stcConfig.enMD    = BtMode2; //模式2为16位重载方式
    //Bt???
    if (Ok != Bt_Init(TIM2, &stcConfig))
    {
        enResult = Error;
    }
    
    //?????????,????
    Bt_ARRSet(TIM2, u16ArrData);
    Bt_Cnt16Set(TIM2, u16InitCntData);
    Bt_Run(TIM2);  
    
    return enResult;
}



 void send_u16(uint16_t temp)
{
 M0P_UART1->SBUF = ((uint8_t *)&temp)[1];
	while(M0P_UART1->ISR_f.TI == 0);
	M0P_UART1->ISR_f.TI = 0;
	Uart_ClrStatus(UARTCH1,UartTxEmpty); 
	M0P_UART1->SBUF = ((uint8_t *)&temp)[0];
	while(M0P_UART1->ISR_f.TI == 0);
	M0P_UART1->ISR_f.TI = 0;
	Uart_ClrStatus(UARTCH1,UartTxEmpty); 
 }


 
 //串口初始化
 void uart1_init(void)
 {
  uint16_t timer;
    uint32_t pclk;
    stc_clk_config_t stcCfg;       //时钟初始化配置
    stc_uart_config_t  stcConfig;  //uart 总体配置
    stc_uart_irq_cb_t stcUartIrqCb; //uart发送接收中断处理函数接口
    stc_uart_multimode_t stcMulti;  //uart多主机模式及从机地址和地址掩码配置
    stc_uart_baud_config_t stcBaud;  //uart 通道波特率配置
    stc_bt_config_t stcBtConfig;     //Timer 配置结构体定义 
    stc_lpm_config_t stcLpmCfg;      //Low Power Mode 配置结构体定义 
    
    en_uart_mmdorck_t enTb8;  //uart多主机模式地址帧/数据帧或者奇偶校验

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    DDL_ZERO_STRUCT(stcBtConfig);
    DDL_ZERO_STRUCT(stcLpmCfg);
    
    stcLpmCfg.enSLEEPDEEP = SlpDpDisable;

//    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcUartIrqCb);
    DDL_ZERO_STRUCT(stcMulti);
    DDL_ZERO_STRUCT(stcBaud);
    DDL_ZERO_STRUCT(stcBtConfig);
    DDL_ZERO_STRUCT(stcLpmCfg);
    
    stcLpmCfg.enSLEEPDEEP = SlpDpDisable;
    
    //时钟部分配置
//    stcCfg.enClkSrc = ClkXTH;   //使用外部高速时钟
//    stcCfg.enHClkDiv = ClkDiv1;
//    stcCfg.enPClkDiv = ClkDiv1;
//    Clk_Init(&stcCfg);


stcCfg.enClkSrc = ClkRCH;   //使用内部高速时钟
    stcCfg.enHClkDiv = ClkDiv1;
    stcCfg.enPClkDiv = ClkDiv1;
//   // Clk_Init(&stcCfg);	 
//	 
//	
//    
//    Clk_SetRCHFreq(ClkFreq8Mhz);    //系统主频8MHZ
//    Clk_Init(&stcCfg);
		
		
		M0P_CLOCK->RCH_CR = RCH_CR_TRIM_8M_VAL;   //直接配置成8M时钟
	
	while(!(M0P_CLOCK->RCH_CR_f.STABLE));   //等待RCH时钟稳定 
	
	M0P_CLOCK->PERI_CLKEN = 0x90208503;  //使能UART0/1 BT AT WDT TRIM GPIO FLASH时钟
    
    Gpio_InitIO(T1_PORT,T1_PIN,GpioDirIn); 
    Gpio_InitIO(0,3,GpioDirOut);
    Gpio_SetIO(0,3,1);
    
    Gpio_InitIOExt(3,5,GpioDirOut,TRUE,FALSE,FALSE,FALSE);   
    Gpio_InitIOExt(3,6,GpioDirOut,TRUE,FALSE,FALSE,FALSE); 
    
    //通道端口配置
    Gpio_SetFunc_UART1TX_P35();   //P35配置为UART1 的TX
    Gpio_SetFunc_UART1RX_P36();  //P36配置为UART1 的RX

    //外设时钟使能
    Clk_SetPeripheralGate(ClkPeripheralBt,TRUE);//模式0/2可以不使能
    Clk_SetPeripheralGate(ClkPeripheralUart1,TRUE);



    stcUartIrqCb.pfnRxIrqCb = RxIntCallback;
    stcUartIrqCb.pfnTxIrqCb = TxIntCallback;
    stcUartIrqCb.pfnRxErrIrqCb = ErrIntCallback;
    stcConfig.pstcIrqCb = &stcUartIrqCb;
    stcConfig.bTouchNvic = TRUE;
  


    stcConfig.enRunMode = UartMode1;//测试项，更改此处来转换4种模式测试
   

    stcMulti.enMulti_mode = UartNormal;/////<正常工作模式 而非多机通信模式
    enTb8 = DataOrEven;///<多主机模式时，数据帧TB8标记 ,非多主机模式偶校验  
    Uart_SetMMDOrCk(UARTCH1,enTb8);
    stcConfig.pstcMultiMode = &stcMulti;
#if 1  
    stcBaud.bDbaud = 0u;//双倍波特率功能
    stcBaud.u32Baud = 19200u;//更新波特率位置
    stcBaud.u8Mode = UartMode1; //计算波特率需要模式参数
    pclk = Clk_GetPClkFreq();
    timer=Uart_SetBaudRate(UARTCH1,pclk,&stcBaud);

    stcBtConfig.enMD = BtMode2;
    stcBtConfig.enCT = BtTimer;
    Bt_Init(TIM1, &stcBtConfig);//调用basetimer1设置函数产生波特率
    Bt_ARRSet(TIM1,timer);
    Bt_Cnt16Set(TIM1,timer);
    Bt_Run(TIM1);
#endif
    Uart_Init(UARTCH1, &stcConfig);  ///////////////////////////////////////////////////////
    Uart_EnableIrq(UARTCH1,UartRxIrq);  /////////////////////////////////////////////////////////
    Uart_ClrStatus(UARTCH1,UartRxFull); //////////////////////////////////////////////////////////
    Uart_EnableFunc(UARTCH1,UartRx);    //////////////////////////////////////////////////////////
/*#if 0 //休眠模式测试
    while(1 == Gpio_GetIO(T1_PORT,T1_PIN));//注意：如果用户修改进入深度休眠的话，此处不能屏蔽
    Lpm_Config(&stcLpmCfg);
    Lpm_GotoLpmMode();
    Gpio_SetIO(0,3,0);
#endif*/
  }
 

	
static void WdtCallback(void)
{
  Wdt_Feed();
	wd_count = 1;
}
 void init_watchdog(void)
 {
	 
		stc_wdt_config_t  stcWdt_Config;
		stcWdt_Config.u8LoadValue = wdt_wov;//1.64s
    stcWdt_Config.enResetEnable = WINT_EN;//WRESET_EN;WINT_EN////
    stcWdt_Config.pfnWdtIrqCb = WdtCallback;
    
	 
    Clk_SetPeripheralGate(ClkPeripheralWdt,TRUE);//
    Wdt_Init(&stcWdt_Config);
    
    Wdt_Start();
  }

/*********************单片机与串口通信协议*********************************/

//十六进制转ASCII码(温湿度)
void Hex_HT(int t1,int c1)
{
	uint8_t i;
	
	BCDBUF[0] = 't';
	BCDBUF[1] = '=';
	BCDBUF[2] = t1/100%10 + '0';
	BCDBUF[3] = t1/10%10 + '0';
	BCDBUF[4] = '.';
	BCDBUF[5] = t1%10 + '0';
	
	BCDBUF[6] = ',';
	
	BCDBUF[7] = 'h';
	BCDBUF[8] = '=';
	BCDBUF[9] = c1/100%10 + '0';
	BCDBUF[10] = c1/10%10 + '0';
	BCDBUF[11] = '.';
	BCDBUF[12] = c1%10 + '0';
	
	for(i=0;i<13;i++) {
	
		Uart_SendData(UARTCH1,BCDBUF[i]);
	}
	
	Uart_SendData(UARTCH1,0x0a);
}

//十六进制转ASCII码(华氏度，湿度)
void Hex_HF(int f1,int c1)
{
	uint8_t i;
	
	BCDBUF[0] = 'f';
	BCDBUF[1] = '=';
	BCDBUF[2] = f1/1000%10 + '0';
	BCDBUF[3] = f1/100%10 + '0';
	BCDBUF[4] = '.';
	BCDBUF[5] = f1/10%10 + '0';
	
	BCDBUF[6] = ',';
	
	BCDBUF[7] = 'h';
	BCDBUF[8] = '=';
	BCDBUF[9] = c1/100%10 + '0';
	BCDBUF[10] = c1/10%10 + '0';
	BCDBUF[11] = '.';
	BCDBUF[12] = c1%10 + '0';
	
	for(i=0;i<13;i++) {
	
		Uart_SendData(UARTCH1,BCDBUF[i]);
	}
	
	Uart_SendData(UARTCH1,0x0a);
}



//格式(第一个温度，第二个湿度): set t=X.X,h=X.X
//校准温湿度
uint8_t update()
{
		//判断数据开始是否为set
		//if((PortReceiveBuffer[0] == 0x73) &&(PortReceiveBuffer[1] == 0x65) && (PortReceiveBuffer[2] == 0x74))	{
			//将收到的温度字符串数据转为整数型
			temp_data[0] = PortReceiveBuffer[6] - 0x30;
			temp_data[1] = PortReceiveBuffer[7] - 0x30;
			temp_data[2] = PortReceiveBuffer[8] - 0x30;
			
			//转化湿度
			temp_data[3] = PortReceiveBuffer[12] - 0x30;
			temp_data[4] = PortReceiveBuffer[13] - 0x30;
			temp_data[5] = PortReceiveBuffer[14] - 0x30;
			
			//误差
			update_t = temp_data[0] * 10 + temp_data[2];
			update_rh = temp_data[3] * 10 + temp_data[5];
			
			update_flag = 1;
		//}
		rx_flag = 0;
		PortWriteBufferPointer = 0;
		
		Uart_SendData(UARTCH1,'o');
		Uart_SendData(UARTCH1,'k');
		Uart_SendData(UARTCH1,0x0a);
				
		return update_flag;
}

//串口获取当前温湿度
void Get_HT(int t,int rh,int f)
{
	JH_Read_CTdata(CT_data);
	rh = CT_data[0]*1000/1024/1024; 
	t = (CT_data[1]*200*10/1024/1024-500) ;
	f = t*18+3200;
	
	//if((PortReceiveBuffer[0] == 0x67) && (PortReceiveBuffer[1] == 0x65) && (PortReceiveBuffer[2] == 0x74)){
		if(update_flag ==0) { 
			
			if(cf_change == 0) {
			
				Hex_HT(t,rh);
			}
			
			else
			{
				Hex_HF(f,rh);
			}
		}
		
		
		else
		{
			rh += update_rh; 
			t += update_t;
			
			if(cf_change == 0) {
			
				Hex_HT(t,rh);
			}
			
			else
			{
				Hex_HF(f,rh);
			}
		}
		
		rx_flag = 0;
		PortWriteBufferPointer = 0;
		
		Uart_SendData(UARTCH1,'o');
		Uart_SendData(UARTCH1,'k');
		Uart_SendData(UARTCH1,0x0a);
	//}
}


//设置自动发送
void Set_auto(void)
{
	//if((PortReceiveBuffer[0] == 0x61) && (PortReceiveBuffer[1] == 0x75) && (PortReceiveBuffer[2] == 0x74) && (PortReceiveBuffer[3] == 0x6F)) {
	
		auto_flag = PortReceiveBuffer[5] - 0x30;
	//}
	
	rx_flag = 0;
	PortWriteBufferPointer = 0;
	
	Uart_SendData(UARTCH1,'o');
	Uart_SendData(UARTCH1,'k');
	Uart_SendData(UARTCH1,0x0a);
}

/*********************单片机与串口通信协议*********************************/

//延时函数
void delay_ms(unsigned int x)
{
  unsigned int i,j;
	
  for(i=x;i>0;i--)
   for(j=110;j>0;j--);        
}

//获取传感器数据，并处理
uint8_t Get_Deal_T_Data(void)
{
	JH_Read_CTdata(CT_data);
	
	c1 = CT_data[0]*1000/1024/1024; 
	t1 = (CT_data[1]*200*10/1024/1024-500) ;
	
	return 1;
}

uint8_t Get_Deal_F_Data(void)
{
	JH_Read_CTdata(CT_data);
	
	c1 = CT_data[0]*1000/1024/1024; 
	t1 = (CT_data[1]*200*10/1024/1024-500) ;
	f1 = t1*18+3200;
	
	return 1;
}

uint8_t Get_Deal_T_UpData(void)
{
	JH_Read_CTdata(CT_data);
	
	c1 = CT_data[0]*1000/1024/1024 + update_rh; 
	t1 = (CT_data[1]*200*10/1024/1024-500) + update_t;
	
	return 1;
}

uint8_t Get_Deal_F_UpData(void)
{
	JH_Read_CTdata(CT_data);
	
	c1 = CT_data[0]*1000/1024/1024 + update_rh; 
	t1 = (CT_data[1]*200*10/1024/1024-500) + update_t;
	f1 = t1*18+3200;
	
	return 1;
}

//进入低功耗前，GPIO的配置
void Gpio_Config(void)
{
	Gpio_InitIOExt(3,4,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(0,1,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(0,2,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(0,3,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(2,6,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(2,4,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(2,3,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(3,1,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(2,7,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	Gpio_InitIOExt(2,5,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	
	
	
//	Gpio_SetIO(1,5,1);
//	Gpio_SetIO(1,4,1);	
}



int32_t main(void)
{  
	uint8_t ret = 0;
	uart1_init();	
	//Clk_SetFunc(ClkFuncSwdPinIOEn,TRUE);   //把SWD作为GPIO
	JH_Init();			
	init_watchdog();    
	BtTimerInit();
		
	Gpio_InitIOExt(3,2, GpioDirIn, TRUE, FALSE, FALSE, FALSE);	
	Gpio_EnableIrq(3,2, GpioIrqFalling);
	EnableNvic(PORT3_IRQn, DDL_IRQ_LEVEL_DEFAULT, TRUE);
	
	//将USB插入IO口配置为内部下拉
	Gpio_InitIOExt(2,5,GpioDirIn, FALSE, TRUE, FALSE, FALSE);
	
	delay_ms(500);
	
	while(1)
	{		
	//开机
		if((Gpio_GetIO(3,2)==0) && (init_state ==0))
		{
			delay_ms(10);
			while(Gpio_GetIO(3,2)==0)
			{
				if(count  > 1000)
				{
					ONOFF_Pin_Output_High();
					PCF8562_Init();
				
					key_long_press = 1;
					key_short_press = 1;
					init_state = 1;
					count  = 0;
					break;
				}
			}
			while(!Gpio_GetIO(3,2));
		}
			
//华氏度显示
	if((key_short_press == 1) && (cf_change == 0))
	{
		count = 0;
		delay_count = 0;
		if(Gpio_GetIO(3,2)==0)
		{
			delay_ms(40);
			while(Gpio_GetIO(3,2)==0)
			{
				if(count < 800)
				{
					if(Get_Deal_F_Data()) {
						Data_F(f1,pcf_data);
						Data_RH(c1,pcf_data);
						PCF8562_Display(pcf_data,6);
			
						count  = 0;
						cf_change = 1;
					}
					
					break;
				}
			}
			delay_ms(40);
			while(!Gpio_GetIO(3,2)) {
			
				if(count > 1000) {
						
						PCF8562_OFF();
						ONOFF_Pin_Output_Low();	
					}
			}
			
		}	
	}

//摄氏度显示
	if((key_short_press == 1) && (cf_change == 1))
	{
		count = 0;
		delay_count = 0;
		if(Gpio_GetIO(3,2)==0)
		{
			delay_ms(40);
			while(Gpio_GetIO(3,2)==0)
			{
				if(count < 800)
				{
					if(Get_Deal_T_Data()) {	
						Data_T(t1,pcf_data);
						Data_RH(c1,pcf_data);
						PCF8562_Display(pcf_data,6);
					
						count  = 0;
						cf_change = 0;
					}
					
					break;
				}	
			}
			delay_ms(40);
			while(!Gpio_GetIO(3,2)) {
			
				if(count > 1000) {
						
						PCF8562_OFF();
						ONOFF_Pin_Output_Low();	
					}
			}
		}		
	}

	//关机
	if((init_state == 1) && (key_long_press == 1))
	{
		count = 0;
		if(Gpio_GetIO(3,2)==0)
		{
			delay_ms(40);
			while(Gpio_GetIO(3,2)==0)
			{
				
				if(count  > 1000 )
				{
					PCF8562_OFF();
					ONOFF_Pin_Output_Low();
					
					count  = 0;
					key_long_press = 0;
					key_short_press = 1;
					break;
				}
			}
			while(!Gpio_GetIO(3,2));
		}		
	}
	
	//电池供电模式	
	if(Gpio_GetIO(2,5) == 0) {
//定时器定时输出串口和显示
	//if(aht10_read_tick == 2000) {
		if(wd_count == 1) {
			//aht10_read_tick = 0;
			
			if(update_flag == 0) {
			//华氏度显示
			if(cf_change == 1) {

				if(Get_Deal_F_Data()) {
					Data_F(f1,pcf_data);
					Data_RH(c1,pcf_data);
					PCF8562_Display(pcf_data,6);
				}
			}

			//摄氏度显示
			if(cf_change == 0) {

				if(Get_Deal_T_Data()) {
					Data_T(t1,pcf_data);
					Data_RH(c1,pcf_data);
					PCF8562_Display(pcf_data,6);	
				}					
			}
		}//end flag = 0

		//这里用来判断是否接收到串口发送的数据
		if(rx_flag == 1) {

			//发送当前温湿度
			if((PortReceiveBuffer[0] == 0x67) && (PortReceiveBuffer[1] == 0x65) && (PortReceiveBuffer[2] == 0x74)) {
			
				Get_HT(t1,c1,f1);
			}
			
			
			//设置自动发送
			if((PortReceiveBuffer[0] == 0x61) && (PortReceiveBuffer[1] == 0x75) && (PortReceiveBuffer[2] == 0x74) && (PortReceiveBuffer[3] == 0x6F)) {
			
				Set_auto();
			}
			
			//校准误差
			if((PortReceiveBuffer[0] == 0x73) &&(PortReceiveBuffer[1] == 0x65) && (PortReceiveBuffer[2] == 0x74)) {
			
				ret = update();
			}
		}	
			if(ret == 1) {
			
				//华氏度显示
				if(cf_change == 1) {

					if(Get_Deal_F_UpData()) {
						Data_F(f1,pcf_data);
						Data_RH(c1,pcf_data);
						PCF8562_Display(pcf_data,6);	
					}						
				}

				//摄氏度显示
				if(cf_change == 0) {

					if(Get_Deal_T_UpData()) {
						Data_T(t1,pcf_data);
						Data_RH(c1,pcf_data);
						PCF8562_Display(pcf_data,6);	
					}						
				}
			}
		wd_count = 0;
		
		__NOP();
		Gpio_Config();
		SCB->SCR |= 0x4;
		__NOP();
		__WFI();	
		
		} //end  if(wd_count)
	}
	
	//USB供电模式
	if(Gpio_GetIO(2,5) == 1) {
//定时器定时输出串口和显示
		if(aht10_read_tick >= 1000) {
			aht10_read_tick = 0;
			
			if(update_flag == 0) {
			//华氏度显示
			if(cf_change == 1) {

				if(Get_Deal_F_Data()) {
					Data_F(f1,pcf_data);
					Data_RH(c1,pcf_data);
					PCF8562_Display(pcf_data,6);
				}
				
				if(auto_flag ==1) {
						Hex_HF(f1,c1);
					}		
			}

			//摄氏度显示
			if(cf_change == 0) {

				if(Get_Deal_T_Data()) {
					Data_T(t1,pcf_data);
					Data_RH(c1,pcf_data);
					PCF8562_Display(pcf_data,6);
				}
				
				if(auto_flag ==1) {
					Hex_HT(t1,c1);
				}		
			}
		}//end flag = 0

		//这里用来判断是否接收到串口发送的数据
		if(rx_flag == 1) {

			//发送当前温湿度
			if((PortReceiveBuffer[0] == 0x67) && (PortReceiveBuffer[1] == 0x65) && (PortReceiveBuffer[2] == 0x74)) {
			
				Get_HT(t1,c1,f1);
			}
			
			
			//设置自动发送
			if((PortReceiveBuffer[0] == 0x61) && (PortReceiveBuffer[1] == 0x75) && (PortReceiveBuffer[2] == 0x74) && (PortReceiveBuffer[3] == 0x6F)) {
			
				Set_auto();
			}
			
			//校准误差
			if((PortReceiveBuffer[0] == 0x73) &&(PortReceiveBuffer[1] == 0x65) && (PortReceiveBuffer[2] == 0x74)) {
			
				ret = update();
			}
		}	
			if(ret == 1) {
			
				//华氏度显示
				if(cf_change == 1) {

					if(Get_Deal_F_UpData()) {
						Data_F(f1,pcf_data);
						Data_RH(c1,pcf_data);
						PCF8562_Display(pcf_data,6);
					}
					
					if(auto_flag ==1) {
						
						Hex_HF(f1,c1);	
					}					
				}

				//摄氏度显示
				if(cf_change == 0) {

					if(Get_Deal_T_UpData()) {
						Data_T(t1,pcf_data);
						Data_RH(c1,pcf_data);
						PCF8562_Display(pcf_data,6);
					}
					
					if(auto_flag ==1) {
						
						Hex_HT(t1,c1);	
					}		
				}
			}
		} //end  if(aht_read)
	}
	
	}//end while	

}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


