#include "basic_tim.h"
#include "protocol.h"
#include "Serial_port.h"

TIM_HandleTypeDef TIM_TimeBaseStructure;

// 基本定时器 TIMx,x[6,7]中断优先级配置
static void TIMx_NVIC_Configuration(void)
{
	//设置抢占优先级，子优先级
	HAL_NVIC_SetPriority(BASIC_TIM_IRQn, 2, 0);
	// 设置中断来源
	HAL_NVIC_EnableIRQ(BASIC_TIM_IRQn);
}

static void TIM_Mode_Config(void)
{
    //定时器使能
	BASIC_TIM_CLK_ENABLE(); 

	TIM_TimeBaseStructure.Instance = BASIC_TIM;

	TIM_TimeBaseStructure.Init.Period = BASIC_PERIOD_COUNT - 1;

	TIM_TimeBaseStructure.Init.Prescaler = BASIC_PRESCALER_COUNT - 1;	
    TIM_TimeBaseStructure.Init.CounterMode = TIM_COUNTERMODE_UP;           // 向上计数
    TIM_TimeBaseStructure.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;     // 时钟分频

	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

	// 开启定时器更新中断
	HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);	
}

// 初始化基本定时器定时，1ms产生一次中断
void TIMx_Configuration(void)
{
	TIMx_NVIC_Configuration();	
  
	TIM_Mode_Config();
  
#if PID_ASSISTANT_EN
  uint32_t temp = GET_BASIC_TIM_PERIOD(); // 计算周期，单位ms 
  set_computer_value(SEND_PERIOD_CMD, CURVES_CH1, &temp, 1); // 给通道 1 发送目标值
#endif

}
