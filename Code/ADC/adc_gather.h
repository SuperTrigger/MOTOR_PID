#ifndef __ADC_GATHER_H
#define	__ADC_GATHER_H

#include "main.h"

// ADC 序号宏定义
#define CURR_ADC                        ADC1
#define CURR_ADC_CLK_ENABLE()           __HAL_RCC_ADC1_CLK_ENABLE()

#define ADC_VBUS_IRQ                    ADC_IRQn
#define ADC_VBUS_IRQHandler             ADC_IRQHandler

#define VREF                            3.3f     // 参考电压
#define ADC_NUM_MAX                     256      // ADC 转换结果缓冲区最大值

#define GET_ADC_VDC_VAL(val)            ((float)val/(float)4096.0 * VREF) // 电压值
  
// 电流采集 
// ADC GPIO 宏定义
#define CURR_ADC_GPIO_PORT              GPIOC
#define CURR_ADC_GPIO_PIN               GPIO_PIN_0
#define CURR_ADC_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define CURR_ADC_CHANNEL                ADC_CHANNEL_10
#define CURR_ADC_DR_ADDR                ((uint32_t)ADC1 + 0x4c) // ADC DR寄存器宏定义

// ADC DMA 通道宏定义
#define CURR_ADC_DMA_CLK_ENABLE()       __HAL_RCC_DMA1_CLK_ENABLE()
#define CURR_ADC_DMA_STREAM             DMA1_Channel1
#define ADC_DMA_IRQ                     DMA1_Channel1_IRQn
#define ADC_DMA_IRQ_Handler             DMA1_Channel1_IRQHandler
#define GET_ADC_CURR_VAL(val)           (((float)val)/(float)8.0/(float)0.02 * (float)1000.0) 

// 电源电压采集 
#define VBUS_GPIO_PORT                  GPIOC
#define VBUS_GPIO_PIN                   GPIO_PIN_1
#define VBUS_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()
#define VBUS_ADC_CHANNEL                ADC_CHANNEL_11
#define GET_VBUS_VAL(val)               (((float)val - (float)1.24) * (float)37.0) // 电压最大值

extern DMA_HandleTypeDef DMA_Init_Handle;
extern ADC_HandleTypeDef ADC_Handle;

void ADC_Init(void);
int32_t get_curr_val(void);
float get_vbus_val(void);

#endif


