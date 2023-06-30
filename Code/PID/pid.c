#include "pid.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "Serial_port.h"

//����ȫ�ֱ���
_pid pid_location; // λ��
_pid pid_speed; // �ٶ�

//  PID������ʼ��
void PID_param_init(void)
{
	// λ����س�ʼ������ 
  pid_location.target_val = PER_CYCLE_PULSES;				 
  pid_location.actual_val = 0.0;
  pid_location.err = 0.0;
  pid_location.err_last = 0.0;
  pid_location.integral = 0.0;

  pid_location.Kp = 0.045;
  pid_location.Ki = 0.0;
  pid_location.Kd = 0.0;

  // �ٶ���س�ʼ������ 
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
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     // ��ͨ�� 1 ���� P I D ֵ
  
  pid_temp[0] = pid_speed.Kp;
  pid_temp[1] = pid_speed.Ki;
  pid_temp[2] = pid_speed.Kd;
  set_computer_value(SEND_P_I_D_CMD, CURVES_CH2, pid_temp, 3);     // ��ͨ�� 2 ���� P I D ֵ
#endif
} 

// ����Ŀ��ֵ 
void set_pid_target(_pid *pid, float temp_val)
{
  pid->target_val = temp_val; // ���õ�ǰ��Ŀ��ֵ
}

// ��ȡĿ��ֵ
float get_pid_target(_pid *pid)
{
  return pid->target_val; // ���õ�ǰ��Ŀ��ֵ
}

// ���ñ��������֡�΢��ϵ��
void set_p_i_d(_pid *pid, float p, float i, float d)
{
  pid->Kp = p;    // ���ñ���ϵ�� P
  pid->Ki = i;    // ���û���ϵ�� I
  pid->Kd = d;    // ����΢��ϵ�� D
}

// λ��PID�㷨ʵ��
float location_pid_realize(_pid *pid, float actual_val)
{
  // ����Ŀ��ֵ��ʵ��ֵ�����
  pid->err=pid->target_val-actual_val;

  // �趨�ջ����� 
  if((pid->err >= -20) && (pid->err <= 20))
  {
    pid->err = 0;
    pid->integral = 0;
  }
  
  pid->integral += pid->err; // ����ۻ�

  // PID�㷨ʵ��
  pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);

  // ����
  pid->err_last=pid->err;
  
  // ���ص�ǰʵ��ֵ
  return pid->actual_val;
}

// �ٶ�PID�㷨ʵ��
float speed_pid_realize(_pid *pid, float actual_val)
{
  // ����Ŀ��ֵ��ʵ��ֵ�����
  pid->err=pid->target_val-actual_val;

  if((pid->err<0.2f )&& (pid->err>-0.2f))
    pid->err = 0.0f;

  pid->integral += pid->err; // ����ۻ�

  // PID�㷨ʵ��
  pid->actual_val = pid->Kp*pid->err+pid->Ki*pid->integral+pid->Kd*(pid->err-pid->err_last);

  // ����
  pid->err_last=pid->err;
  
  // ���ص�ǰʵ��ֵ
  return pid->actual_val;
}
