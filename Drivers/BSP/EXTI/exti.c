#include "./BSP/EXTI/exti.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/ENCODER/encoder.h"
extern int over_count_tim6;
extern TIM_HandleTypeDef htim_6;
void exti_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    __HAL_RCC_GPIOC_CLK_ENABLE();                                 
	__HAL_RCC_GPIOA_CLK_ENABLE(); 
    gpio_init_struct.Pin = GPIO_PIN_5;                   /* LED0?? */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING ;            /* ???? */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                    /* ?- */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* ?? */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);       /* ???LED0?? */
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);                               

    gpio_init_struct.Pin = GPIO_PIN_2;                   /* ���������� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                    /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;          /* ���� */
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);       /* ��ʼ��LED0���� */                                      
}
void SR04_Init(void)
{
	GPIO_InitTypeDef gpio_init_struct = {0};//����һ�����ų�ʼ���Ľṹ��  
	__HAL_RCC_GPIOC_CLK_ENABLE();//ʹ��GPIOA��ʱ��
	gpio_init_struct.Pin = GPIO_PIN_8;   
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;        /* ����ʽ���ù��� */
	gpio_init_struct.Pull = GPIO_PULLUP;          /* ���� */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &gpio_init_struct);
	gpio_init_struct.Pin = GPIO_PIN_9;   
	gpio_init_struct.Mode = GPIO_MODE_IT_RISING_FALLING;        /* ����ʽ���ù��� */
    HAL_GPIO_Init(GPIOC, &gpio_init_struct);       /* ???LED0?? */
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,2,0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); 
}
void EXTI9_5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==GPIO_PIN_5)
	{	
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)==1)
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
			delay_ms(100);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
			delay_ms(200);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
			delay_ms(100);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET);
			delay_ms(150);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		printf("�ж��Ѳ���___________________________-------\r\n");
	}
}