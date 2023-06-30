#include "motor.h"
#include "Serial_port.h"
#include "encoder.h"
#include "pid.h"
#include "basic_tim.h"
#include "motor_tim.h"

static motor_dir_t direction  = MOTOR_FWD;      // ��¼����
static uint16_t dutyfactor = 0;                 // ��¼ռ�ձ�
static uint8_t is_motor_en = 0;                 // ���ʹ��

#define TARGET_SPEED_MAX      200    // Ŀ���ٶȵ����ֵ r/m

static void sd_gpio_config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  // ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� 
	SHUTDOWN_GPIO_CLK_ENABLE();
  
  // ����IO��ʼ�� 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	//������������ 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	// ѡ��Ҫ���Ƶ�GPIO����	
	GPIO_InitStruct.Pin = SHUTDOWN_PIN;
	
  HAL_GPIO_Init(SHUTDOWN_GPIO_PORT, &GPIO_InitStruct);
}

// �����ʼ��
void motor_init(void)
{
  Motor_TIMx_Configuration();    
  sd_gpio_config();
}

// ���õ���ٶ�
void set_motor_speed(uint16_t v)
{
  v = (v > PWM_PERIOD_COUNT) ? PWM_PERIOD_COUNT : v;     // ���޴���
  
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // �����ٶ�
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // �����ٶ�
  }
}

// ���õ������
void set_motor_direction(motor_dir_t dir)
{
  direction = dir;

  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // �����ٶ�
    SET_REV_COMPAER(0);              // �����ٶ�
  }
  else
  {
    SET_FWD_COMPAER(0);              // �����ٶ�
    SET_REV_COMPAER(dutyfactor);     // �����ٶ�
  }
}

// ʹ�ܵ��
void set_motor_enable(void)
{
  is_motor_en = 1;	
  MOTOR_ENABLE_SD(); //��ʹ�����ŲŽ�
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

// ���õ��
void set_motor_disable(void)
{
  is_motor_en = 0;
  MOTOR_DISABLE_SD();
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}

// ���λ��ʽ PID ����ʵ��(��ʱ����)
void motor_pid_control(void)
{
  static uint32_t location_timer = 0;   // λ�û�����
  
  if (is_motor_en == 1)                 // �����ʹ��״̬�²Ž��п��ƴ���
  {
    static int32_t Capture_Count = 0;   // ��ǰʱ���ܼ���ֵ
    static int32_t Last_Count = 0;      // ��һʱ���ܼ���ֵ
    float cont_val = 0;                 // ��ǰ����ֵ
    
    Capture_Count = __HAL_TIM_GET_COUNTER(&TIM_EncoderHandle) + (Encoder_Overflow_Count * ENCODER_TIM_PERIOD);
    
    if (location_timer++%2 == 0)
    {
      cont_val = location_pid_realize(&pid_location, Capture_Count); // ���� PID ����
      
      // Ŀ���ٶ����޴��� 
      if (cont_val > TARGET_SPEED_MAX)
      {
        cont_val = TARGET_SPEED_MAX;
      }
      else if (cont_val < -TARGET_SPEED_MAX)
      {
        cont_val = -TARGET_SPEED_MAX;
      }
      
      set_pid_target(&pid_speed, cont_val); // �趨�ٶȵ�Ŀ��ֵ
      
    #if defined(PID_ASSISTANT_EN)
      int32_t temp = cont_val;
      set_computer_value(SEND_TARGET_CMD, CURVES_CH2, &temp, 1); // ��ͨ�� 2 ����Ŀ��ֵ
    #endif
    }
    
    // ת��ת�� = ��λʱ���ڵļ���ֵ / �������ֱܷ��� * ʱ��ϵ��  
    int32_t actual_speed = 0; // ʵ�ʲ���ٶ�
    actual_speed = ((float)(Capture_Count - Last_Count) / ENCODER_TOTAL_RESOLUTION / REDUCTION_RATIO) / (GET_BASIC_TIM_PERIOD()/1000.0/60.0);
    
    // ��¼��ǰ�ܼ���ֵ������һʱ�̼���ʹ�� 
    Last_Count = Capture_Count;
    
    cont_val = speed_pid_realize(&pid_speed, actual_speed); // ���� PID ����
    
    if (cont_val > 0) // �жϵ������
    {
      set_motor_direction(MOTOR_FWD);
    }
    else
    {
      cont_val = -cont_val;
      set_motor_direction(MOTOR_REV);
    }
    
    cont_val = (cont_val > PWM_MAX_PERIOD_COUNT) ? PWM_MAX_PERIOD_COUNT : cont_val; // �ٶ����޴���
    set_motor_speed(cont_val); // ���� PWM ռ�ձ�
    
  #if defined(PID_ASSISTANT_EN)
    set_computer_value(SEND_FACT_CMD, CURVES_CH2, &actual_speed, 1);      // ��ͨ�� 2 ����ʵ��ֵ
    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &Capture_Count, 1);     // ��ͨ�� 1 ����ʵ��ֵ
  #else
    printf("ʵ��ֵ�ٶȣ�%d. ʵ��λ��ֵ��%d, Ŀ��λ��ֵ��%.0f\n", actual_speed, Capture_Count, get_pid_target(&pid_location));      // ��ӡʵ��ֵ��Ŀ��ֵ
  #endif
  }
}

