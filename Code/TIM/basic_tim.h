#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "main.h"

#define BASIC_TIM           		    TIM6
#define BASIC_TIM_CLK_ENABLE()   	    __HAL_RCC_TIM6_CLK_ENABLE()

#define BASIC_TIM_IRQn				    TIM6_DAC_IRQn
#define BASIC_TIM_IRQHandler    	    TIM6_DAC_IRQHandler

	
//PID��������:50ms����һ��--Ƶ��20Hz
#define BASIC_PERIOD_MS         (50) //50ms

#define BASIC_PERIOD_COUNT      (BASIC_PERIOD_MS*100)

#define BASIC_PRESCALER_COUNT   (720) //100Khz

// ��������������ڶ�ʱ��ʱ��ԴTIMxCLK=72MHz��Ԥ��Ƶ��Ϊ��720-1 ����� 
#define SET_BASIC_TIM_PERIOD(T)     __HAL_TIM_SET_AUTORELOAD(&TIM_TimeBaseStructure, (T)*100 - 1)    // ���ö�ʱ�������ڣ�1~1000ms��
#define GET_BASIC_TIM_PERIOD()      ((__HAL_TIM_GET_AUTORELOAD(&TIM_TimeBaseStructure)+1)/100.0f)     // ��ȡ��ʱ�������ڣ���λms

extern TIM_HandleTypeDef TIM_TimeBaseStructure;
void TIMx_Configuration(void);


#endif
