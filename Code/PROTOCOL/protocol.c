#include "protocol.h"
#include "motor.h"
#include "pid.h"
#include "Serial_port.h"
#include "basic_tim.h"

struct prot_frame_parser_t
{
  uint8_t *recv_ptr;
  uint16_t r_oft;
  uint16_t w_oft;
  uint16_t frame_len;
  uint16_t found_frame_head;
};

static struct prot_frame_parser_t parser;

static uint8_t recv_buf[PROT_FRAME_LEN_RECV];

// 计算校验和
uint8_t check_sum(uint8_t init, uint8_t *ptr, uint8_t len )
{
  uint8_t sum = init;
  while(len--)
  {
    sum += *ptr;
    ptr++;
  }
  return sum;
}

// 得到帧类型（帧命令）
static uint8_t get_frame_type(uint8_t *frame, uint16_t head_oft)
{
  return (frame[(head_oft + CMD_INDEX_VAL) % PROT_FRAME_LEN_RECV] & 0xFF);
}

// 得到帧长度
static uint16_t get_frame_len(uint8_t *frame, uint16_t head_oft)
{
  return ((frame[(head_oft + LEN_INDEX_VAL + 0) % PROT_FRAME_LEN_RECV] <<  0) |
          (frame[(head_oft + LEN_INDEX_VAL + 1) % PROT_FRAME_LEN_RECV] <<  8) |
          (frame[(head_oft + LEN_INDEX_VAL + 2) % PROT_FRAME_LEN_RECV] << 16) |
          (frame[(head_oft + LEN_INDEX_VAL + 3) % PROT_FRAME_LEN_RECV] << 24)); // 合成帧长度
}

// 获取 crc-16 校验值
static uint8_t get_frame_checksum(uint8_t *frame, uint16_t head_oft, uint16_t frame_len)
{
    return (frame[(head_oft + frame_len - 1) % PROT_FRAME_LEN_RECV]);
}

// 查找帧头
static int32_t recvbuf_find_header(uint8_t *buf, uint16_t ring_buf_len, uint16_t start, uint16_t len)
{
  uint16_t i = 0;

  for (i = 0; i < (len - 3); i++)
  {
      if (((buf[(start + i + 0) % ring_buf_len] <<  0) |
            (buf[(start + i + 1) % ring_buf_len] <<  8) |
            (buf[(start + i + 2) % ring_buf_len] << 16) |
            (buf[(start + i + 3) % ring_buf_len] << 24)) == FRAME_HEADER)
      {
          return ((start + i) % ring_buf_len);
      }
  }
  return -1;
}

// 计算为解析的数据长度
static int32_t recvbuf_get_len_to_parse(uint16_t frame_len, uint16_t ring_buf_len,uint16_t start, uint16_t end)
{
  uint16_t unparsed_data_len = 0;

  if (start <= end)
      unparsed_data_len = end - start;
  else
      unparsed_data_len = ring_buf_len - start + end;

  if (frame_len > unparsed_data_len)
      return 0;
  else
      return unparsed_data_len;
}

// 接收数据写入缓冲区
static void recvbuf_put_data(uint8_t *buf, uint16_t ring_buf_len, uint16_t w_oft,
        uint8_t *data, uint16_t data_len)
{
  if ((w_oft + data_len) > ring_buf_len)               // 超过缓冲区尾
  {
      uint16_t data_len_part = ring_buf_len - w_oft;     // 缓冲区剩余长度

      /* 数据分两段写入缓冲区*/
      memcpy(buf + w_oft, data, data_len_part);                         // 写入缓冲区尾
      memcpy(buf, data + data_len_part, data_len - data_len_part);      // 写入缓冲区头
  }
  else
      memcpy(buf + w_oft, data, data_len);    // 数据写入缓冲区
}

// 查询帧类型（命令）
static uint8_t protocol_frame_parse(uint8_t *data, uint16_t *data_len)
{
  uint8_t frame_type = CMD_NONE;
  uint16_t need_to_parse_len = 0;
  int16_t header_oft = -1;
  uint8_t checksum = 0;
  
  need_to_parse_len = recvbuf_get_len_to_parse(parser.frame_len, PROT_FRAME_LEN_RECV, parser.r_oft, parser.w_oft);    // 得到为解析的数据长度
  if (need_to_parse_len < 9)  
      return frame_type;

  // 还未找到帧头，需要进行查找
  if (0 == parser.found_frame_head)
  {
    header_oft = recvbuf_find_header(parser.recv_ptr, PROT_FRAME_LEN_RECV, parser.r_oft, need_to_parse_len);
    if (0 <= header_oft)
    {
      // 已找到帧头
      parser.found_frame_head = 1;
      parser.r_oft = header_oft;
    
      // 确认是否可以计算帧长
      if (recvbuf_get_len_to_parse(parser.frame_len, PROT_FRAME_LEN_RECV,
              parser.r_oft, parser.w_oft) < 9)
          return frame_type;
    }
    else 
    {
        // 未解析的数据中依然未找到帧头，丢掉此次解析过的所有数据
        parser.r_oft = ((parser.r_oft + need_to_parse_len - 3) % PROT_FRAME_LEN_RECV);
        return frame_type;
    }
  }
    
  // 计算帧长，并确定是否可以进行数据解析
  if (0 == parser.frame_len) 
  {
    parser.frame_len = get_frame_len(parser.recv_ptr, parser.r_oft);
    if(need_to_parse_len < parser.frame_len)
        return frame_type;
  }

  // 帧头位置确认，且未解析的数据超过帧长，可以计算校验和
  if ((parser.frame_len + parser.r_oft - PROT_FRAME_LEN_CHECKSUM) > PROT_FRAME_LEN_RECV)
  {
    // 数据帧被分为两部分，一部分在缓冲区尾，一部分在缓冲区头 
    checksum = check_sum(checksum, parser.recv_ptr + parser.r_oft, 
            PROT_FRAME_LEN_RECV - parser.r_oft);
    checksum = check_sum(checksum, parser.recv_ptr, parser.frame_len -
            PROT_FRAME_LEN_CHECKSUM + parser.r_oft - PROT_FRAME_LEN_RECV);
  }
  else 
  {
    // 数据帧可以一次性取完
    checksum = check_sum(checksum, parser.recv_ptr + parser.r_oft, parser.frame_len - PROT_FRAME_LEN_CHECKSUM);
  }

  if (checksum == get_frame_checksum(parser.recv_ptr, parser.r_oft, parser.frame_len))
  {
    // 校验成功，拷贝整帧数据 
    if ((parser.r_oft + parser.frame_len) > PROT_FRAME_LEN_RECV) 
    {
      // 数据帧被分为两部分，一部分在缓冲区尾，一部分在缓冲区头
      uint16_t data_len_part = PROT_FRAME_LEN_RECV - parser.r_oft;
      memcpy(data, parser.recv_ptr + parser.r_oft, data_len_part);
      memcpy(data + data_len_part, parser.recv_ptr, parser.frame_len - data_len_part);
    }
    else 
    {
      // 数据帧可以一次性取完
      memcpy(data, parser.recv_ptr + parser.r_oft, parser.frame_len);
    }
    *data_len = parser.frame_len;
    frame_type = get_frame_type(parser.recv_ptr, parser.r_oft);

    // 丢弃缓冲区中的命令帧
    parser.r_oft = (parser.r_oft + parser.frame_len) % PROT_FRAME_LEN_RECV;
  }
  else
  {
    // 校验错误，说明之前找到的帧头只是偶然出现的废数据
    parser.r_oft = (parser.r_oft + 1) % PROT_FRAME_LEN_RECV;
  }
  parser.frame_len = 0;
  parser.found_frame_head = 0;

  return frame_type;
}

// 接收数据处理
void protocol_data_recv(uint8_t *data, uint16_t data_len)
{
    recvbuf_put_data(parser.recv_ptr, PROT_FRAME_LEN_RECV, parser.w_oft, data, data_len);    // 接收数据
    parser.w_oft = (parser.w_oft + data_len) % PROT_FRAME_LEN_RECV;                          // 计算写偏移
}

// 初始化接收协议
int32_t protocol_init(void)
{
    memset(&parser, 0, sizeof(struct prot_frame_parser_t));
    
    /* 初始化分配数据接收与解析缓冲区*/
    parser.recv_ptr = recv_buf;
  
    return 0;
}

// 接收的数据处理
int8_t receiving_process(void)
{
  uint8_t frame_data[128];         // 要能放下最长的帧
  uint16_t frame_len = 0;          // 帧长度
  uint8_t cmd_type = CMD_NONE;     // 命令类型
  packet_head_t packet;
  
  while(1)
  {
    cmd_type = protocol_frame_parse(frame_data, &frame_len);
    switch (cmd_type)
    {
      case CMD_NONE:
      {
        return -1;
      }

      case SET_P_I_D_CMD:
      {
        type_cast_t temp_p, temp_i, temp_d;

        packet.ch = frame_data[CHX_INDEX_VAL];
        
        temp_p.i = COMPOUND_32BIT(&frame_data[13]);
        temp_i.i = COMPOUND_32BIT(&frame_data[17]);
        temp_d.i = COMPOUND_32BIT(&frame_data[21]);
        
        if (packet.ch == CURVES_CH1)
        {
          set_p_i_d(&pid_location, temp_p.f, temp_i.f, temp_d.f);    // 设置 P I D
        }
        else if (packet.ch == CURVES_CH2)
        {
          set_p_i_d(&pid_speed, temp_p.f, temp_i.f, temp_d.f);    // 设置 P I D
        }
      }
      break;

      case SET_TARGET_CMD:
      {
        int target_temp = COMPOUND_32BIT(&frame_data[13]);    // 得到数据
        packet.ch = frame_data[CHX_INDEX_VAL];
        
        // 只设置速度的目标值，电流的目标置是由速度pid的输出决定的 
        if (packet.ch == CURVES_CH1)    
        {
          set_pid_target(&pid_location, target_temp);    // 设置目标值
        }
      }
      break;
      
      case START_CMD:
      {
        set_motor_enable();              // 启动电机
      }
      break;
      
      case STOP_CMD:
      {
        set_motor_disable();              // 停止电机
      }
      break;
      
      case RESET_CMD:
      {
        HAL_NVIC_SystemReset();          // 复位系统
      }
      break;
      
      case SET_PERIOD_CMD:
      {
        uint32_t temp = COMPOUND_32BIT(&frame_data[13]);     // 周期数
        packet.ch = frame_data[CHX_INDEX_VAL];
        
        if (packet.ch == CURVES_CH1)
        {
          SET_BASIC_TIM_PERIOD(temp);                             // 设置定时器周期1~1000ms
        }
        else if (packet.ch == CURVES_CH2)
        {
          SET_BASIC_TIM_PERIOD(temp);                             // 设置定时器周期1~1000ms
        }
      }
      break;

      default: 
        return -1;
    }
  }
}

// 设置上位机的值
void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num)
{
  uint8_t sum = 0;    // 校验和
  num *= 4;           // 一个参数 4 个字节
  
  static packet_head_t set_packet;
  
  set_packet.head = FRAME_HEADER;     // 包头 0x59485A53
  set_packet.len  = 0x0B + num;      // 包长
  set_packet.ch   = ch;              // 设置通道
  set_packet.cmd  = cmd;             // 设置命令
  
  sum = check_sum(0, (uint8_t *)&set_packet, sizeof(set_packet));       // 计算包头校验和
  sum = check_sum(sum, (uint8_t *)data, num);                           // 计算参数校验和
  
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&set_packet, sizeof(set_packet), 0xFFFFF);    // 发送数据头
  HAL_UART_Transmit(&UartHandle, (uint8_t *)data, num, 0xFFFFF);                          // 发送参数
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&sum, sizeof(sum), 0xFFFFF);                  // 发送校验和
}
