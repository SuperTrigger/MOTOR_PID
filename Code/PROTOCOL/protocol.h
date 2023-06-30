
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "main.h"

#ifdef _cplusplus
extern "C" {
#endif   

// ���ݽ��ջ�������С 
#define PROT_FRAME_LEN_RECV  128

// У�����ݵĳ��� 
#define PROT_FRAME_LEN_CHECKSUM    1

// ����ͷ�ṹ�� 
typedef __packed struct
{
    uint32_t head;    // ��ͷ
    uint8_t ch;       // ͨ��
    uint32_t len;     // ������
    uint8_t cmd;      // ����
    uint8_t sum;      // У���
}packet_head_t;

// �����壨��������ת���� 
typedef union
{
    float f;
    int i;
}type_cast_t;

#define FRAME_HEADER     0x59485A53    // ֡ͷ

// ͨ���궨�� 
#define CURVES_CH1      0x01
#define CURVES_CH2      0x02
#define CURVES_CH3      0x03
#define CURVES_CH4      0x04
#define CURVES_CH5      0x05

// ָ��(��λ�� -> ��λ��) 
#define SEND_TARGET_CMD      0x01     // ������λ��ͨ����Ŀ��ֵ
#define SEND_FACT_CMD        0x02     // ����ͨ��ʵ��ֵ
#define SEND_P_I_D_CMD       0x03     // ���� PID ֵ��ͬ����λ����ʾ��ֵ��
#define SEND_START_CMD       0x04     // ��������ָ�ͬ����λ����ť״̬��
#define SEND_STOP_CMD        0x05     // ����ָֹͣ�ͬ����λ����ť״̬��
#define SEND_PERIOD_CMD      0x06     // �������ڣ�ͬ����λ����ʾ��ֵ��

// ָ��(��λ�� -> ��λ��) 
#define SET_P_I_D_CMD        0x10     // ���� PID ֵ
#define SET_TARGET_CMD       0x11     // ����Ŀ��ֵ
#define START_CMD            0x12     // ����ָ��
#define STOP_CMD             0x13     // ָֹͣ��
#define RESET_CMD            0x14     // ��λָ��
#define SET_PERIOD_CMD       0x15     // ��������

// ��ָ�� 
#define CMD_NONE             0xFF     // ��ָ��

// ����ֵ�궨�� 
#define HEAD_INDEX_VAL       0x3u     // ��ͷ����ֵ��4�ֽڣ�
#define CHX_INDEX_VAL        0x4u     // ͨ������ֵ��1�ֽڣ�
#define LEN_INDEX_VAL        0x5u     // ��������ֵ��4�ֽڣ�
#define CMD_INDEX_VAL        0x9u     // ��������ֵ��1�ֽڣ�

#define EXCHANGE_H_L_BIT(data)      ((((data) << 24) & 0xFF000000) |\
                                     (((data) <<  8) & 0x00FF0000) |\
                                     (((data) >>  8) & 0x0000FF00) |\
                                     (((data) >> 24) & 0x000000FF))     // �����ߵ��ֽ�

#define COMPOUND_32BIT(data)        (((*(data-0) << 24) & 0xFF000000) |\
                                     ((*(data-1) << 16) & 0x00FF0000) |\
                                     ((*(data-2) <<  8) & 0x0000FF00) |\
                                     ((*(data-3) <<  0) & 0x000000FF))      // �ϳ�Ϊһ����
                                     
// �������ݴ���
void protocol_data_recv(uint8_t *data, uint16_t data_len);

// ��ʼ������Э��
int32_t protocol_init(void);
int8_t receiving_process(void);
void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num);

#ifdef _cplusplus
}
#endif   

#endif
