#ifndef __ENCOEDER_H
#define	__ENCOEDER_H

#include "main.h"

// 定时器选择 
#define ENCODER_TIM                           TIM4
#define ENCODER_TIM_CLK_ENABLE()  			      __HAL_RCC_TIM4_CLK_ENABLE()
#define ENCODER_TIM_AF_CLK_ENABLE()  		      __HAL_AFIO_REMAP_TIM4_ENABLE()

// 定时器溢出值 		
#define ENCODER_TIM_PERIOD                     65535
// 定时器预分频值 
#define ENCODER_TIM_PRESCALER                  0      

// 定时器中断
#define ENCODER_TIM_IRQn                       TIM4_IRQn
#define ENCODER_TIM_IRQHandler                 TIM4_IRQHandler

// 编码器接口引脚
#define ENCODER_TIM_CH1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define ENCODER_TIM_CH1_GPIO_PORT              GPIOD
#define ENCODER_TIM_CH1_PIN                    GPIO_PIN_12

#define ENCODER_TIM_CH2_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define ENCODER_TIM_CH2_GPIO_PORT              GPIOD
#define ENCODER_TIM_CH2_PIN                    GPIO_PIN_13

// 编码器接口倍频数 
#define ENCODER_MODE                           TIM_ENCODERMODE_TI12

// 编码器接口输入捕获通道相位设置 
#define ENCODER_IC1_POLARITY                   TIM_ICPOLARITY_RISING
#define ENCODER_IC2_POLARITY                   TIM_ICPOLARITY_RISING

// 编码器物理分辨率 
#define ENCODER_RESOLUTION                     500

// 经过倍频之后的总分辨率 
#if (ENCODER_MODE == TIM_ENCODERMODE_TI12)
  #define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 4)  /* 4倍频后的总分辨率 */
#else
  #define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 2)  /* 2倍频后的总分辨率 */
#endif

// 减速电机减速比 
#define REDUCTION_RATIO                        30

extern __IO int16_t Encoder_Overflow_Count;
extern TIM_HandleTypeDef TIM_EncoderHandle;

void Encoder_Init(void);

#endif   


