#include "basic_tim.h"
#include "protocol.h"
#include "Serial_port.h"

TIM_HandleTypeDef TIM_TimeBaseStructure;

// ������ʱ�� TIMx,x[6,7]�ж����ȼ�����
static void TIMx_NVIC_Configuration(void)
{
	//������ռ���ȼ��������ȼ�
	HAL_NVIC_SetPriority(BASIC_TIM_IRQn, 2, 0);
	// �����ж���Դ
	HAL_NVIC_EnableIRQ(BASIC_TIM_IRQn);
}

static void TIM_Mode_Config(void)
{
    //��ʱ��ʹ��
	BASIC_TIM_CLK_ENABLE(); 

	TIM_TimeBaseStructure.Instance = BASIC_TIM;

	TIM_TimeBaseStructure.Init.Period = BASIC_PERIOD_COUNT - 1;

	TIM_TimeBaseStructure.Init.Prescaler = BASIC_PRESCALER_COUNT - 1;	
    TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;           // ���ϼ���
    TIM_TimeBaseStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;     // ʱ�ӷ�Ƶ

	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

	// ������ʱ�������ж�
	HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);	
}

// ��ʼ��������ʱ����ʱ��1ms����һ���ж�
void TIMx_Configuration(void)
{
	TIMx_NVIC_Configuration();	
  
	TIM_Mode_Config();
  
#if PID_ASSISTANT_EN
  uint32_t temp = GET_BASIC_TIM_PERIOD(); // �������ڣ���λms 
  set_computer_value(SEND_PERIOD_CMD, CURVES_CH1, &temp, 1); // ��ͨ�� 1 ����Ŀ��ֵ
#endif

}
