#include "stm32f10x.h"
#include "bsp_usart.h"
#include "delay.h"
#include "bsp_aht20.h"
#include "bsp_led.h"


int main(void)
{

	u32 CT_data[2]={0};
    volatile float  hum=0,temp=0;     
     
	USART_Config();			//USART1初始化
	LED_GPIO_Config();		//LED端口初始化
    delay_init();	    	//延时函数初始化	  
    temphum_init();         //初始化温湿度传感器
	
	while(1)
	{
        AHT20_Read_CTdata(CT_data);       //不经过CRC校验，直接读取AHT20的温度和湿度数据 

        hum = CT_data[0]*100*10/1024/1024;  //计算得到湿度值（放大了10倍）
        temp = CT_data[1]*200*10/1024/1024-500;//计算得到温度值（放大了10倍）

        printf("湿度:%.1f%%\r\n",(hum/10));
        printf("温度:%.1f度\r\n",(temp/10));
        printf("\r\n");
        
        /*绿灯闪烁提示串口发送状态*/
        green_led_on;
        delay_ms(1000);
		green_led_off;
        delay_ms(1000);
	 }
}
