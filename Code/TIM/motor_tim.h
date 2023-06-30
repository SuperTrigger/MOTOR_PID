#ifndef __MOTOR_TIM_H
#define	__MOTOR_TIM_H

#include "main.h"

#define PWM_TIM                        	    TIM1
#define PWM_TIM_GPIO_AF_ENALBE()            __HAL_AFIO_REMAP_TIM1_ENABLE();
#define PWM_TIM_CLK_ENABLE()  			    __HAL_RCC_TIM1_CLK_ENABLE()

#define PWM_CHANNEL_1                       TIM_CHANNEL_1
#define PWM_CHANNEL_2                       TIM_CHANNEL_2

#define PWM_PERIOD_COUNT                    (3600)

#define PWM_PRESCALER_COUNT                 (2)

// 最大比较值 
#define PWM_MAX_PERIOD_COUNT                (PWM_PERIOD_COUNT - 100)

// 定时器通道
#define PWM_TIM_CH1_GPIO_CLK()              __HAL_RCC_GPIOE_CLK_ENABLE();
#define PWM_TIM_CH1_GPIO_PORT               GPIOE
#define PWM_TIM_CH1_PIN                     GPIO_PIN_9

#define PWM_TIM_CH2_GPIO_CLK()              __HAL_RCC_GPIOE_CLK_ENABLE();
#define PWM_TIM_CH2_GPIO_PORT               GPIOE
#define PWM_TIM_CH2_PIN                     GPIO_PIN_11

extern TIM_HandleTypeDef  DCM_TimeBaseStructure;

extern void Motor_TIMx_Configuration(void);
extern void TIM1_SetPWM_pulse(uint32_t channel,int compare);

#endif 
