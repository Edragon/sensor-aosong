#ifndef __BSP_LED_H		//条件编译的作用，头文件被多次调用时，防止被重复编译
#define __BSP_LED_H

#include "stm32f10x.h"


/******************************************************
		定义宏，提高程序的可移植性
*******************************************************/						
// 绿灯
#define LED_G_GPIO_PIN 		GPIO_Pin_0
// 红灯
#define LED_R_GPIO_PIN		GPIO_Pin_5
// 蓝灯
#define LED_B_GPIO_PIN 		GPIO_Pin_1
// 端口
#define LED_GPIO_PORT		GPIOB
// 时钟
#define LED_GPIO_CLK		RCC_APB2Periph_GPIOB



// 点亮绿灯
#define			green_led_on		GPIO_ResetBits(LED_GPIO_PORT, LED_G_GPIO_PIN);
// 熄灭绿灯
#define			green_led_off		GPIO_SetBits(LED_GPIO_PORT, LED_G_GPIO_PIN);
// 点亮红灯
#define			red_led_on			GPIO_ResetBits(LED_GPIO_PORT, LED_R_GPIO_PIN);
// 熄灭红灯
#define			red_led_off			GPIO_SetBits(LED_GPIO_PORT, LED_R_GPIO_PIN);
// 点亮蓝灯
#define			blue_led_on			GPIO_ResetBits(LED_GPIO_PORT, LED_B_GPIO_PIN);
//熄灭蓝灯
#define 		blue_led_off		GPIO_SetBits(LED_GPIO_PORT, LED_B_GPIO_PIN);




/******************************************************
				函数声明
*******************************************************/	
void LED_GPIO_Config(void);
void Close_LED(void);


#endif /*__BSP_LED_H*/

