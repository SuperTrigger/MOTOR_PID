#ifndef __MOTOR_H
#define	__MOTOR_H

#include "main.h"

//引脚定义 // EN 脚
#define SHUTDOWN_PIN                  GPIO_PIN_9
#define SHUTDOWN_GPIO_PORT            GPIOE
#define SHUTDOWN_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOE_CLK_ENABLE()


// 电机使能脚 
#define MOTOR_ENABLE_SD()   HAL_GPIO_WritePin(SHUTDOWN_GPIO_PORT, SHUTDOWN_PIN, GPIO_PIN_SET)      // 高电平打开-高电平使能 
#define MOTOR_DISABLE_SD()  HAL_GPIO_WritePin(SHUTDOWN_GPIO_PORT, SHUTDOWN_PIN, GPIO_PIN_RESET)    // 低电平关断-低电平禁用

// 电机方向控制枚举 
typedef enum
{
    MOTOR_FWD = 0,
    MOTOR_REV,
}motor_dir_t;

#define PER_CYCLE_PULSES    (ENCODER_TOTAL_RESOLUTION * REDUCTION_RATIO)    // 电机旋转一圈可以捕获的脉冲

// 设置速度（占空比） 
#define SET_FWD_COMPAER(ChannelPulse)     TIM1_SetPWM_pulse(PWM_CHANNEL_1, ChannelPulse)    // 设置比较寄存器的值
#define SET_REV_COMPAER(ChannelPulse)     TIM1_SetPWM_pulse(PWM_CHANNEL_2, ChannelPulse)    // 设置比较寄存器的值

// 使能输出 
#define MOTOR_FWD_ENABLE()      HAL_TIM_PWM_Start(&DCM_TimeBaseStructure, PWM_CHANNEL_1);
#define MOTOR_REV_ENABLE()      HAL_TIM_PWM_Start(&DCM_TimeBaseStructure, PWM_CHANNEL_2);

// 禁用输出 
#define MOTOR_FWD_DISABLE()     HAL_TIM_PWM_Stop(&DCM_TimeBaseStructure, PWM_CHANNEL_1);
#define MOTOR_REV_DISABLE()     HAL_TIM_PWM_Stop(&DCM_TimeBaseStructure, PWM_CHANNEL_2);

void motor_init(void);
void set_motor_speed(uint16_t v);
void set_motor_direction(motor_dir_t dir);
void set_motor_enable(void);
void set_motor_disable(void);
void show_help(void);
void deal_serial_data(void);
void motor_pid_control(void);

#endif 
