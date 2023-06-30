#include "motor_tim.h"

void TIM_SetTIMxCompare(TIM_TypeDef *TIMx,uint32_t channel,uint32_t compare);
void TIM_SetPWM_period(TIM_TypeDef* TIMx,uint32_t TIM_period);

// ����TIM�������PWMʱ�õ���I/O
static void TIMx_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStruct;

    PWM_TIM_CH1_GPIO_CLK();
	PWM_TIM_CH2_GPIO_CLK();

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	//������������ 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	// ���ø���
	PWM_TIM_GPIO_AF_ENALBE();
	
	// ѡ��Ҫ���Ƶ�GPIO����	
	GPIO_InitStruct.Pin = PWM_TIM_CH1_PIN;
	// ���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO
    HAL_GPIO_Init(PWM_TIM_CH1_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PWM_TIM_CH2_PIN;	
    HAL_GPIO_Init(PWM_TIM_CH2_GPIO_PORT, &GPIO_InitStruct);
	
}

TIM_HandleTypeDef  DCM_TimeBaseStructure;

static void TIM_PWMOUTPUT_Config(void)
{
    TIM_OC_InitTypeDef TIM_OCInitStructure;  
	
    // ʹ�ܶ�ʱ��
    PWM_TIM_CLK_ENABLE();
	
    DCM_TimeBaseStructure.Instance = PWM_TIM;

	DCM_TimeBaseStructure.Init.Period = PWM_PERIOD_COUNT - 1;
	// ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(PWM_PRESCALER_COUNT+1)
    DCM_TimeBaseStructure.Init.Prescaler = PWM_PRESCALER_COUNT - 1;	
        
    //������ʽ*/
    DCM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;
    //����ʱ�ӷ�Ƶ
    DCM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	//��ʼ����ʱ��
    HAL_TIM_PWM_Init(&DCM_TimeBaseStructure);
  
	//PWMģʽ����
    TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
	TIM_OCInitStructure.Pulse = 500;
	TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStructure.OCNPolarity = TIM_OCPOLARITY_HIGH;
	TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
	TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    TIM_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE;
	
	//����PWMͨ��
    HAL_TIM_PWM_ConfigChannel(&DCM_TimeBaseStructure, &TIM_OCInitStructure, PWM_CHANNEL_1);
	//��ʼ���PWM
	HAL_TIM_PWM_Start(&DCM_TimeBaseStructure,PWM_CHANNEL_1);
	
	//��������
    TIM_OCInitStructure.Pulse = 0; // Ĭ��ռ�ձ�Ϊ50%
	//����PWMͨ��
    HAL_TIM_PWM_ConfigChannel(&DCM_TimeBaseStructure, &TIM_OCInitStructure, PWM_CHANNEL_2);
	//��ʼ���PWM
	HAL_TIM_PWM_Start(&DCM_TimeBaseStructure, PWM_CHANNEL_2);
}

// ����TIMͨ����ռ�ձ�
void TIM1_SetPWM_pulse(uint32_t channel,int compare)
{
	switch(channel)
	{
		case TIM_CHANNEL_1:  __HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure, TIM_CHANNEL_1, compare);break;
		case TIM_CHANNEL_2:	 __HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure, TIM_CHANNEL_2, compare);break;
		case TIM_CHANNEL_3:	 __HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure, TIM_CHANNEL_3, compare);break;
		case TIM_CHANNEL_4:	 __HAL_TIM_SET_COMPARE(&DCM_TimeBaseStructure, TIM_CHANNEL_4, compare);break;
	}
}

// ��ʼ������ͨ�ö�ʱ��
void Motor_TIMx_Configuration(void)
{
	TIMx_GPIO_Config();
  
    TIM_PWMOUTPUT_Config();
}
