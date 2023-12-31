#include "motor.h"
#include "Serial_port.h"
#include "encoder.h"
#include "pid.h"
#include "basic_tim.h"
#include "motor_tim.h"

static motor_dir_t direction  = MOTOR_FWD;      // 记录方向
static uint16_t dutyfactor = 0;                 // 记录占空比
static uint8_t is_motor_en = 0;                 // 电机使能

#define TARGET_SPEED_MAX      200    // 目标速度的最大值 r/m

static void sd_gpio_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  // 定时器通道功能引脚端口时钟使能 
	SHUTDOWN_GPIO_CLK_ENABLE();
  
  // 引脚IO初始化 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	//设置引脚速率 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	// 选择要控制的GPIO引脚	
	GPIO_InitStruct.Pin = SHUTDOWN_PIN;
	
  HAL_GPIO_Init(SHUTDOWN_GPIO_PORT, &GPIO_InitStruct);
}

// 电机初始化
void motor_init(void)
{
  Motor_TIMx_Configuration();    
  sd_gpio_config();
}

// 设置电机速度
void set_motor_speed(uint16_t v)
{
  v = (v > PWM_PERIOD_COUNT) ? PWM_PERIOD_COUNT : v;     // 上限处理
  
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

// 设置电机方向
void set_motor_direction(motor_dir_t dir)
{
  direction = dir;

  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
    SET_REV_COMPAER(0);              // 设置速度
  }
  else
  {
    SET_FWD_COMPAER(0);              // 设置速度
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

// 使能电机
void set_motor_enable(void)
{
  is_motor_en = 1;	
  MOTOR_ENABLE_SD(); //有使能引脚才接
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

// 禁用电机
void set_motor_disable(void)
{
  is_motor_en = 0;
  MOTOR_DISABLE_SD();
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}

// 电机位置式 PID 控制实现(定时调用)
void motor_pid_control(void)
{
  static uint32_t location_timer = 0;   // 位置环周期
  
  if (is_motor_en == 1)                 // 电机在使能状态下才进行控制处理
  {
    static int32_t Capture_Count = 0;   // 当前时刻总计数值
    static int32_t Last_Count = 0;      // 上一时刻总计数值
    float cont_val = 0;                 // 当前控制值
    
    Capture_Count = __HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    
    if (location_timer++%2 == 0)
    {
      cont_val = location_pid_realize(&pid_location, Capture_Count); // 进行 PID 计算
      
      // 目标速度上限处理 
      if (cont_val > TARGET_SPEED_MAX)
      {
        cont_val = TARGET_SPEED_MAX;
      }
      else if (cont_val < -TARGET_SPEED_MAX)
      {
        cont_val = -TARGET_SPEED_MAX;
      }
      
      set_pid_target(&pid_speed, cont_val); // 设定速度的目标值
      
    #if defined(PID_ASSISTANT_EN)
      int32_t temp = cont_val;
      set_computer_value(SEND_TARGET_CMD, CURVES_CH2, &temp, 1); // 给通道 2 发送目标值
    #endif
    }
    
    // 转轴转速 = 单位时间内的计数值 / 编码器总分辨率 * 时间系数  
    int32_t actual_speed = 0; // 实际测得速度
    actual_speed = ((float)(Capture_Count - Last_Count) / ENCODER_TOTAL_RESOLUTION / REDUCTION_RATIO) / (GET_BASIC_TIM_PERIOD()/1000.0/60.0);
    
    // 记录当前总计数值，供下一时刻计算使用 
    Last_Count = Capture_Count;
    
    cont_val = speed_pid_realize(&pid_speed, actual_speed); // 进行 PID 计算
    
    if (cont_val > 0) // 判断电机方向
    {
      set_motor_direction(MOTOR_FWD);
    }
    else
    {
      cont_val = -cont_val;
      set_motor_direction(MOTOR_REV);
    }
    
    cont_val = (cont_val > PWM_MAX_PERIOD_COUNT) ? PWM_MAX_PERIOD_COUNT : cont_val; // 速度上限处理
    set_motor_speed(cont_val); // 设置 PWM 占空比
    
  #if defined(PID_ASSISTANT_EN)
    set_computer_value(SEND_FACT_CMD, CURVES_CH2, &actual_speed, 1);      // 给通道 2 发送实际值
    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &Capture_Count, 1);     // 给通道 1 发送实际值
  #else
    printf("实际值速度：%d. 实际位置值：%d, 目标位置值：%.0f\n", actual_speed, Capture_Count, get_pid_target(&pid_location));      // 打印实际值和目标值
  #endif
  }
}


