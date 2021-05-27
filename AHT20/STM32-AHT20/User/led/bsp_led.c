// bsp: board support package 板级支持包
#include "bsp_led.h"



/* LED灯初始化配置 */
void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; //定义GPIO_InitTypeDef结构体
	
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//配置成员,变为输出模式
	/*绿灯*/
	GPIO_InitStruct.GPIO_Pin = LED_G_GPIO_PIN; //配置位(I/O口)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; //配置输出速度
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct); //初始化GPIO
	
	/*红灯*/
	GPIO_InitStruct.GPIO_Pin = LED_R_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	/*蓝灯*/
	GPIO_InitStruct.GPIO_Pin = LED_B_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
	
	/* 关闭led灯 */
	green_led_off;
	red_led_off;
	blue_led_off;
	
}
