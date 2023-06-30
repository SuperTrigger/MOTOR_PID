#ifndef __ENCOEDER_H
#define	__ENCOEDER_H

#include "main.h"

// ��ʱ��ѡ�� 
#define ENCODER_TIM                           TIM4
#define ENCODER_TIM_CLK_ENABLE()  			      __HAL_RCC_TIM4_CLK_ENABLE()
#define ENCODER_TIM_AF_CLK_ENABLE()  		      __HAL_AFIO_REMAP_TIM4_ENABLE()

// ��ʱ�����ֵ 		
#define ENCODER_TIM_PERIOD                     65535
// ��ʱ��Ԥ��Ƶֵ 
#define ENCODER_TIM_PRESCALER                  0      

// ��ʱ���ж�
#define ENCODER_TIM_IRQn                       TIM4_IRQn
#define ENCODER_TIM_IRQHandler                 TIM4_IRQHandler

// �������ӿ�����
#define ENCODER_TIM_CH1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define ENCODER_TIM_CH1_GPIO_PORT              GPIOD
#define ENCODER_TIM_CH1_PIN                    GPIO_PIN_12

#define ENCODER_TIM_CH2_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define ENCODER_TIM_CH2_GPIO_PORT              GPIOD
#define ENCODER_TIM_CH2_PIN                    GPIO_PIN_13

// �������ӿڱ�Ƶ�� 
#define ENCODER_MODE                           TIM_ENCODERMODE_TI12

// �������ӿ����벶��ͨ����λ���� 
#define ENCODER_IC1_POLARITY                   TIM_ICPOLARITY_RISING
#define ENCODER_IC2_POLARITY                   TIM_ICPOLARITY_RISING

// ����������ֱ��� 
#define ENCODER_RESOLUTION                     500

// ������Ƶ֮����ֱܷ��� 
#if (ENCODER_MODE == TIM_ENCODERMODE_TI12)
  #define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 4)  /* 4��Ƶ����ֱܷ��� */
#else
  #define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 2)  /* 2��Ƶ����ֱܷ��� */
#endif

// ���ٵ�����ٱ� 
#define REDUCTION_RATIO                        30

extern __IO int16_t Encoder_Overflow_Count;
extern TIM_HandleTypeDef TIM_EncoderHandle;

void Encoder_Init(void);

#endif   


