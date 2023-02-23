#include "./BSP/TIMER/gtim.h"


TIM_HandleTypeDef g_timx_cnt_chy_handle;        /* ��ʱ��x��� */

 /* ͨ�ö�ʱ��ͨ��y ������� ��ʼ������ */
void gtim_timx_cnt_chy_init(uint16_t psc)
{
    TIM_SlaveConfigTypeDef tim_salve_config = {0};
    
    g_timx_cnt_chy_handle.Instance = TIM2;
    g_timx_cnt_chy_handle.Init.Prescaler = psc;
    g_timx_cnt_chy_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_timx_cnt_chy_handle.Init.Period = 65535;
    HAL_TIM_IC_Init(&g_timx_cnt_chy_handle);
    
    tim_salve_config.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;//�ⲿʱ��ģʽ1
    tim_salve_config.InputTrigger = TIM_TS_TI1F_ED;//�����غ��½��ض�����
    tim_salve_config.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;//���ϼ���
    tim_salve_config.TriggerFilter = 0;//�˲�������
    HAL_TIM_SlaveConfigSynchro(&g_timx_cnt_chy_handle, &tim_salve_config);

    HAL_TIM_IC_Start(&g_timx_cnt_chy_handle, TIM_CHANNEL_1);
}

/* ��ʱ�� ���벶�� MSP��ʼ������ */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM8)
    {
        GPIO_InitTypeDef gpio_init_struct;
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_TIM8_CLK_ENABLE();

        gpio_init_struct.Pin = GPIO_PIN_6;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;        /* ����ʽ���ù��� */
        gpio_init_struct.Pull = GPIO_PULLDOWN;          /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;  /* ���� */
        HAL_GPIO_Init(GPIOC, &gpio_init_struct);
    }
}






