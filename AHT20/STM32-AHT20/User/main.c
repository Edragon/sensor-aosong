#include "stm32f10x.h"
#include "bsp_usart.h"
#include "delay.h"
#include "bsp_aht20.h"
#include "bsp_led.h"


int main(void)
{

	u32 CT_data[2]={0};
    volatile float  hum=0,temp=0;     
     
	USART_Config();			//USART1��ʼ��
	LED_GPIO_Config();		//LED�˿ڳ�ʼ��
    delay_init();	    	//��ʱ������ʼ��	  
    temphum_init();         //��ʼ����ʪ�ȴ�����
	
	while(1)
	{
        AHT20_Read_CTdata(CT_data);       //������CRCУ�飬ֱ�Ӷ�ȡAHT20���¶Ⱥ�ʪ������ 

        hum = CT_data[0]*100*10/1024/1024;  //����õ�ʪ��ֵ���Ŵ���10����
        temp = CT_data[1]*200*10/1024/1024-500;//����õ��¶�ֵ���Ŵ���10����

        printf("ʪ��:%.1f%%\r\n",(hum/10));
        printf("�¶�:%.1f��\r\n",(temp/10));
        printf("\r\n");
        
        /*�̵���˸��ʾ���ڷ���״̬*/
        green_led_on;
        delay_ms(1000);
		green_led_off;
        delay_ms(1000);
	 }
}
