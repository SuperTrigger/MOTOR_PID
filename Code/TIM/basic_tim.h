#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "main.h"

#define BASIC_TIM           		    TIM6
#define BASIC_TIM_CLK_ENABLE()   	    __HAL_RCC_TIM6_CLK_ENABLE()

#define BASIC_TIM_IRQn				    TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    	    TIM6_DAC_IRQHandler

	
//PID计算周期:50ms计算一次--频率20Hz
#define BASIC_PERIOD_MS         (50) //50ms

#define BASIC_PERIOD_COUNT      (BASIC_PERIOD_MS*100)

#define BASIC_PRESCALER_COUNT   (720) //100Khz

// 以下两宏仅适用于定时器时钟源TIMxCLK=72MHz，预分频器为：720-1 的情况 
#define SET_BASIC_TIM_PERIOD(T)     __HAL_TIM_SET_AUTORELOAD(&TIM_TimeBaseStructure, (T)*100 - 1)    // 设置定时器的周期（1~1000ms）
#define GET_BASIC_TIM_PERIOD()      ((__HAL_TIM_GET_AUTORELOAD(&TIM_TimeBaseStructure)+1)/100.0f)     // 获取定时器的周期，单位ms

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
void TIMx_Configuration(void);


#endif
