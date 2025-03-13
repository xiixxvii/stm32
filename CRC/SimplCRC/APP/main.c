#include "SWM320.h"

#include <string.h>

void SerialInit(void);

int main(void)
{
	uint32_t i, res;
	char data[] = "Hi from SynWit";
	
	SystemInit();
	
	SerialInit();
	
	CRC_Init(CRC, CRC32_IN8, 0, 0, 0);
	
	for(i = 0; i < strlen("Hi from SynWit"); i++)
		CRC_Write(data[i]);
	
	res = CRC_Result();
	
	if(res == 0xC4D9CB63)
		printf("CRC Pass! Result = 0x%08X\r\n", res);
	else
		printf("CRC Fail! Result = 0x%08X\r\n", res);
   	
	/* ����һ�� */
	CRC->INIVAL = 0;	//���ó�ʼֵ
	
	for(i = 0; i < strlen("Hi from SynWit"); i++)
		CRC_Write(data[i]);
	
	res = CRC_Result();
	
	if(res == 0xC4D9CB63)
		printf("CRC Pass! Result = 0x%08X\r\n", res);
	else
		printf("CRC Fail! Result = 0x%08X\r\n", res);
	
	while(1==1)
	{
	}
}

void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2����ΪUART0��������
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3����ΪUART0�������
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* ��������: fputc()
* ����˵��: printf()ʹ�ô˺������ʵ�ʵĴ��ڴ�ӡ����
* ��    ��: int ch		Ҫ��ӡ���ַ�
*			FILE *f		�ļ����
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
