#include "./BSP/ENCODER/encoder.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/MOTOR_CONTROL/motor_control.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/HR04/HR04.h"
#include "./BSP/uart/uart.h"
extern uint8_t rx_buffer[1];
int over_count_tim6=0;
int right_flag=0;
int left_flag=0;
int flag_r=0;
int flag_l=0;
int Encoder_Overflow_Count_l=0;//���ֶ�ʱ��3�������
int Capture_Count_l = 0;//���ֶ�ʱ��3������ֵ
int last_Count_l = 0;//��һ���ڶ�ʱ��3������ֵ
int Encoder_Overflow_Count_r=0;
int Capture_Count_r = 0;//���ֶ�ʱ��2������ֵ
int last_Count_r = 0;
float Velocity_l = 0;//���ֵ�ǰ�ٶ�
float Velocity_r = 0;
int len=0;
int left_len=0;
int right_len=0;
uint8_t stop=0;
uint8_t slam=0;
uint8_t al_lft=0;
uint8_t stop_set_zero=0;
int stop_set_zero_count=0;
TIM_HandleTypeDef htim_2;//���ֶ�ʱ�����
TIM_HandleTypeDef htim_3;//���ֶ�ʱ�����
extern TIM_HandleTypeDef g_tim4_pwm_chy_handle;//��ʱ���ľ������������PWM���������������
extern TIM_HandleTypeDef g_tim8_pwm_chy_handle;
TIM_HandleTypeDef htim_6;//��ʱ��6���,���ڶ�ʱ�޸�PWMռ�ձȺʹ��ڴ�ӡ�ٶ�
TIM_HandleTypeDef htim_7;
int Target_l=0;//����Ŀ���ٶ�
int Target_V_L=0;
int Target_V_R=0;
int out_r,out_l=0;
float current_r,current_l,tar_pos_r,tar_pos_l=0;
void Encoder_Init_TIM2(void)
{
	GPIO_InitTypeDef gpio_init_struct = {0};//����һ�����ų�ʼ���Ľṹ��  
    TIM_Encoder_InitTypeDef Tim_2_EncoderConfig; //����һ����ʱ��������ģʽ��ʼ���Ľṹ��
	
	__HAL_RCC_GPIOA_CLK_ENABLE();//ʹ��GPIOA��ʱ��
	__HAL_RCC_TIM2_CLK_ENABLE();//ʹ�ܶ�ʱ��2��ʱ��
	gpio_init_struct.Pin = GPIO_PIN_0;   
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;        /* ����ʽ���ù��� */
	gpio_init_struct.Pull = GPIO_PULLUP;          /* ���� */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);
	gpio_init_struct.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);
	
	
    htim_2.Instance = TIM2; 
    htim_2.Init.Prescaler = 0;
    htim_2.Init.CounterMode = TIM_COUNTERMODE_UP; // ģʽΪ���ϼ���
    htim_2.Init.Period = 60000;                   //���ֵΪ60000
    htim_2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  //Ĭ��ΪTIM_CLOCKDIVISION_DIV1
    htim_2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;// ʹ���Զ���װ��
	
	Tim_2_EncoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;//������ģʽ3˫ͨ������
	Tim_2_EncoderConfig.IC1Polarity = TIM_ENCODERINPUTPOLARITY_RISING;//���뼫��ѡ��
	Tim_2_EncoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;//ͨ��ѡ�񣬱�����ģʽ��ֻ��Ϊ��ֵ
	Tim_2_EncoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;//��Ƶϵ������Ϊ1
	Tim_2_EncoderConfig.IC1Filter     = 10;            //�˲�����
	Tim_2_EncoderConfig.IC2Polarity = TIM_ENCODERINPUTPOLARITY_RISING;//���뼫��ѡ��
	Tim_2_EncoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;//ͨ��ѡ�񣬱�����ģʽ��ֻ��Ϊ��ֵ
	Tim_2_EncoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;//��Ƶϵ������Ϊ1
	Tim_2_EncoderConfig.IC2Filter     = 10;            //�˲�����
	
	HAL_TIM_Encoder_Init(&htim_2,  &Tim_2_EncoderConfig);//������ģʽ��ʼ��
	__HAL_TIM_CLEAR_IT(&htim_2,TIM_IT_UPDATE);/* �����жϱ�־λ */
	__HAL_TIM_SET_COUNTER(&htim_2, 0);//���������
    __HAL_TIM_ENABLE_IT(&htim_2,TIM_IT_UPDATE);/* ʹ�ܶ�ʱ���ĸ����¼��ж� */
    __HAL_TIM_URS_ENABLE(&htim_2);/* ���ø����¼�����ԴΪ����ʱ����� */
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 1);/* �����ж����ȼ� */
    HAL_NVIC_EnableIRQ(TIM2_IRQn);/* ʹ�ܶ�ʱ���ж� */
    HAL_TIM_Encoder_Start(&htim_2, TIM_CHANNEL_ALL);/* ʹ�ܱ������ӿ� */
}
void Encoder_Init_TIM3(void)
{
	GPIO_InitTypeDef gpio_init_struct = {0};//����һ�����ų�ʼ���Ľṹ��  
    TIM_Encoder_InitTypeDef Tim_3_EncoderConfig; //����һ����ʱ��������ģʽ��ʼ���Ľṹ��
	
	__HAL_RCC_GPIOA_CLK_ENABLE();//ʹ��GPIOA��ʱ��
	__HAL_RCC_TIM3_CLK_ENABLE();//ʹ�ܶ�ʱ������ʱ��
	gpio_init_struct.Pin = GPIO_PIN_6;   
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;        /* ����ʽ���ù��� */
	gpio_init_struct.Pull = GPIO_PULLUP;          /* ���� */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);
	gpio_init_struct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);
	
	
    htim_3.Instance = TIM3; 
    htim_3.Init.Prescaler = 0;
    htim_3.Init.CounterMode = TIM_COUNTERMODE_UP; // ģʽΪ���ϼ���
    htim_3.Init.Period = 60000;                   //���ֵΪ60000
    htim_3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;  //Ĭ��ΪTIM_CLOCKDIVISION_DIV1
    htim_3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;// ʹ���Զ���װ��
	
	Tim_3_EncoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;//������ģʽ3˫ͨ������
	Tim_3_EncoderConfig.IC1Polarity = TIM_ENCODERINPUTPOLARITY_RISING;//���뼫��ѡ��
	Tim_3_EncoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;//ͨ��ѡ�񣬱�����ģʽ��ֻ��Ϊ��ֵ
	Tim_3_EncoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;//��Ƶϵ������Ϊ1
	Tim_3_EncoderConfig.IC1Filter     = 10;            //�˲�����
	Tim_3_EncoderConfig.IC2Polarity = TIM_ENCODERINPUTPOLARITY_RISING;//���뼫��ѡ��
	Tim_3_EncoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;//ͨ��ѡ�񣬱�����ģʽ��ֻ��Ϊ��ֵ
	Tim_3_EncoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;//��Ƶϵ������Ϊ1
	Tim_3_EncoderConfig.IC2Filter     = 10;            //�˲�����
	
	HAL_TIM_Encoder_Init(&htim_3,  &Tim_3_EncoderConfig);//������ģʽ��ʼ��
	__HAL_TIM_CLEAR_IT(&htim_3,TIM_IT_UPDATE);/* �����жϱ�־λ */
	__HAL_TIM_SET_COUNTER(&htim_3, 0);//���������
    __HAL_TIM_ENABLE_IT(&htim_3,TIM_IT_UPDATE);/* ʹ�ܶ�ʱ���ĸ����¼��ж� */
    __HAL_TIM_URS_ENABLE(&htim_3);/* ���ø����¼�����ԴΪ����ʱ����� */
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 1);/* �����ж����ȼ� */
    HAL_NVIC_EnableIRQ(TIM3_IRQn);/* ʹ�ܶ�ʱ���ж� */
    HAL_TIM_Encoder_Start(&htim_3, TIM_CHANNEL_ALL);/* ʹ�ܱ������ӿ� */
}
 void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
 {
   /* �жϵ�ǰ�������������� */
    if (htim->Instance == TIM6)
    {
		over_count_tim6++;
		len = Get_Len();
		if(stop_set_zero_count==over_count_tim6)
		{
				stop_set_zero=0;
				stop=0;
			    al_lft=0;
		}
		if(over_count_tim6%4==0)
		{
			if(stop==1&&slam==1)
			{
				if(al_lft==0)
				{
					__HAL_TIM_SET_COMPARE(&g_tim8_pwm_chy_handle, TIM_CHANNEL_1, 124);
					al_lft=1;
				}
				else if(al_lft==1)
				{
					left_len=len;
					__HAL_TIM_SET_COMPARE(&g_tim8_pwm_chy_handle, TIM_CHANNEL_1, 24);
					al_lft=2;
				}
				else if(al_lft==2&&stop_set_zero==0)
				{
					right_len=len;
					if(left_len>right_len)
					{
						left_flag=1;
						flag_l=1;
					}
					else
					{
						right_flag=1;
						flag_r=1;
					}
					stop_set_zero=1;
					stop_set_zero_count=over_count_tim6+15;
					__HAL_TIM_SET_COMPARE(&g_tim8_pwm_chy_handle, TIM_CHANNEL_1, 74);
				}
			}
		}
		printf("len=%d___________________________________________________\r\n",len);
		if(len<250&&rx_buffer[0]=='f')
		{
			Target_V_R=0;
			Target_V_L=0;
		}
		else if((len<250&&slam==1)&&(al_lft==0))
		{
			Target_V_R=0;
			Target_V_L=0;
		}
	}	
	else if(htim->Instance == TIM7)
	{
		if(slam==1&&stop==0)
		{
			Target_V_R=200;
			Target_V_L=200;
		}
		Capture_Count_l =__HAL_TIM_GET_COUNTER(&htim_3) + (Encoder_Overflow_Count_l * 60000);/* ��ǰʱ���ܼ���ֵ = ������ֵ + ����������� * ���������ֵ  */
		Velocity_l=(Capture_Count_l-last_Count_l)*3.1416*65/(60*50);
		last_Count_l=Capture_Count_l;		
		Capture_Count_r =__HAL_TIM_GET_COUNTER(&htim_2) + (Encoder_Overflow_Count_r * 60000);/* ��ǰʱ���ܼ���ֵ = ������ֵ + ����������� * ���������ֵ  */
		Velocity_r=((Capture_Count_r-last_Count_r)*3.1416*65)/(60*50);
		last_Count_r=Capture_Count_r;
		printf("��ǰ���ֱ�������ֵ��%d\r\n", Capture_Count_r);
		printf("��ǰ���ֵ��ת�٣�%fm/s\r\n", Velocity_r);
		out_r = Motor_PI_R(Velocity_r,Target_V_R);
		current_l=__HAL_TIM_GET_COUNTER(&htim_3) + (Encoder_Overflow_Count_l * 60000);
		out_l = Motor_PI(Velocity_l,Target_V_L);
		if(right_flag==1&&flag_r==1)
		{
			tar_pos_l=Capture_Count_l+71000;
			flag_r=0;
		}
		if(right_flag==1)
		{
			if(tar_pos_l-current_l<2000&&tar_pos_l-current_l>-2000)
			{
				Target_V_L=0;
				right_flag=0;
			}
			else
			{
				Target_V_L=Pos_PI_L(current_l,tar_pos_l);				
			}
		}
		current_r=__HAL_TIM_GET_COUNTER(&htim_2) + (Encoder_Overflow_Count_r * 60000);
		if(left_flag==1&&flag_l==1)
		{
			tar_pos_r=Capture_Count_r+68000;
			flag_l=0;
		}
		if(left_flag==1)
		{
			if(tar_pos_r-current_r<2000&&tar_pos_r-current_r>-2000)
			{
				Target_V_R=0;
				left_flag=0;
			}
			else
			{
				Target_V_R=Pos_PI_R(current_r,tar_pos_r);				
			}
		}
		if(out_l>0)
		{
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_1, (7199-out_l));
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_2, 7199);
		}
		else if(out_l<=0)
		{
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_1, 7199);
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_2, (7199+out_l));
		}
		if(out_r>0)
		{
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_3, (7199-out_r));
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_4, 7199);
		}
		else if(out_r<=0)
		{
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_3, 7199);
			__HAL_TIM_SET_COMPARE(&g_tim4_pwm_chy_handle, TIM_CHANNEL_4, (7199+out_r));
		}
    }
	if(htim->Instance == TIM3)
	{
		if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim_3))
		{
			Encoder_Overflow_Count_l--;
		}
        else if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim_3)!=1)
		{
			Encoder_Overflow_Count_l++;
	    }
	}
	else if(htim->Instance == TIM2)
	{
		if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim_2))
		{
			Encoder_Overflow_Count_r--;
		}
        else if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim_2)!=1)
		{
			printf("�����жϡ�������������������������������������������������������");
			Encoder_Overflow_Count_r++;
	    }
	}
	//printf("�����ж�\r\n");
}
/* ��ʱ���жϳ�ʼ������ */
void tim6_init(uint16_t arr, uint16_t psc)
{
    htim_6.Instance = TIM6;
    htim_6.Init.Prescaler = psc;
    htim_6.Init.Period = arr;
    HAL_TIM_Base_Init(&htim_6);//��ʱ����ʼ��

    HAL_TIM_Base_Start_IT(&htim_6);//ʹ���ж�
}
void tim7_init(uint16_t arr, uint16_t psc)
{
    htim_7.Instance = TIM7;
    htim_7.Init.Prescaler = psc;
    htim_7.Init.Period = arr;
    HAL_TIM_Base_Init(&htim_7);//��ʱ����ʼ��

    HAL_TIM_Base_Start_IT(&htim_7);//ʹ���ж�
}
/* ��ʱ������MSP��ʼ������ */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM6_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(TIM6_IRQn);
	    __HAL_TIM_SET_COUNTER(&htim_6, 0);//���������
		__HAL_TIM_CLEAR_IT(&htim_6,TIM_IT_UPDATE);/* �����жϱ�־λ */
		__HAL_TIM_ENABLE_IT(&htim_6,TIM_IT_UPDATE);/* ʹ�ܶ�ʱ���ĸ����¼��ж� */
		__HAL_TIM_URS_ENABLE(&htim_6);/* ���ø����¼�����ԴΪ����ʱ����� */
    }
    if (htim->Instance == TIM7)
    {
        __HAL_RCC_TIM7_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM7_IRQn, 2, 1);
        HAL_NVIC_EnableIRQ(TIM7_IRQn);
	    __HAL_TIM_SET_COUNTER(&htim_7, 0);//���������
		__HAL_TIM_CLEAR_IT(&htim_7,TIM_IT_UPDATE);/* �����жϱ�־λ */
		__HAL_TIM_ENABLE_IT(&htim_7,TIM_IT_UPDATE);/* ʹ�ܶ�ʱ���ĸ����¼��ж� */
		__HAL_TIM_URS_ENABLE(&htim_7);/* ���ø����¼�����ԴΪ����ʱ����� */
    }
}


/* ��ʱ��6�жϷ����� */
void TIM6_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim_6);
}
void TIM7_IRQHandler(void)
{
	printf("OK\r\n");
    HAL_TIM_IRQHandler(&htim_7);
}
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim_3);
}
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim_2);
}
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim4_pwm_chy_handle);
}
