#include "Serial_port.h"
#include "protocol.h"

UART_HandleTypeDef UartHandle;

void DEBUG_USART_Config(void)
{ 
  UartHandle.Instance          = DEBUG_USART;
  UartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  
  HAL_UART_Init(&UartHandle);
  __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);  
}

// �����ַ� 
void Usart_SendByte(uint8_t str)
{
  HAL_UART_Transmit(&UartHandle, &str, 1, 1000);
}

// �����ַ���
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
    HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1, 1000);
    k++;
  } while(*(str + k)!='\0');
  
}

//�ض���printf
#if 1
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}

//�ض���fputc����
int fputc(int ch, FILE *f)
{
    while((USART1->SR & 0X40) == 0); //ѭ������,ֱ���������

    USART1->DR = (uint8_t) ch;
    return ch;
}
#endif
