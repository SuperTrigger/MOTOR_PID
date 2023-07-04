#include "pid.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "Serial_port.h"

//定义全局变量
_pid pid_location; // 位置
_pid pid_speed; // 速度

//  PID参数初始化
void PID_param_init(void)
{
	// 位置相关初始化参数 
  pid_location.target_val = PER_CYCLE_PULSES;				 
  pid_location.actual_val = 0.0;
  pid_location.err = 0.0;
  pid_location.err_last = 0.0;
  pid_location.integral = 0.0;

  pid_location.Kp = 0.045;
  pid_location.Ki = 0.0;
  pid_location.Kd = 0.0;

  // 速度相关初始化参数 
  pid_speed.target_val = 100.0;				
  pid_speed.actual_val = 0.0;
  pid_speed.err = 0.0;
  pid_speed.err_last = 0.0;
  pid_speed.integral = 0.0;

  pid_speed.Kp = 5.0;
  pid_speed.Ki = 2.0;
  pid_speed.Kd = 0.0;

#if defined(PID_ASSISTANT_EN)
  float pid_temp[3] = {pid_location.Kp, pid_location.Ki, pid_location.Kd};
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // 给通道 1 发送 P I D 值
  
  pid_temp[0] = pid_speed.Kp;
  pid_temp[1] = pid_speed.Ki;
  pid_temp[2] = pid_speed.Kd;
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH2, pid_temp, 3);     // 给通道 2 发送 P I D 值
#endif
} 

// 设置目标值 
void set_pid_target(_pid *pid, float temp_val)
{
  pid->target_val = temp_val; // 设置当前的目标值
}

// 获取目标值
float get_pid_target(_pid *pid)
{
  return pid->target_val; // 设置当前的目标值
}

// 设置比例、积分、微分系数
void set_p_i_d(_pid *pid, float p, float i, float d)
{
  pid->Kp = p;    // 设置比例系数 P
  pid->Ki = i;    // 设置积分系数 I
  pid->Kd = d;    // 设置微分系数 D
}

// 位置PID算法实现
float location_pid_realize(_pid *pid, float actual_val)
{
  // 计算目标值与实际值的误差
  pid->err=pid->target_val-actual_val;

  // 设定闭环死区 
  if((pid->err >= -20) && (pid->err <= 20))
  {
    pid->err = 0;
    pid->integral = 0;
  }
  
  pid->integral += pid->err; // 误差累积

  // PID算法实现
  pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);

  // 误差传递
  pid->err_last=pid->err;
  
  // 返回当前实际值
  return pid->actual_val;
}
// 速度PID算法实现
float speed_pid_realize(_pid *pid, float actual_val)
{
  // 计算目标值与实际值的误差
  pid->err=pid->target_val-actual_val;

  if((pid->err<0.2f )&& (pid->err>-0.2f))
    pid->err = 0.0f;

  pid->integral += pid->err; // 误差累积

  // PID算法实现
  pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);

  // 误差传递
  pid->err_last=pid->err;
  
  // 返回当前实际值
  return pid->actual_val;
}
