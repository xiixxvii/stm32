#include "SWM320.h"

void SerialInit(void);

int main(void)
{
	uint32_t res, i;
	UART_InitStructure UART_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTA, PIN5, FUNMUX1_UART1_TXD, 0);	//GPIOA.5����ΪUART1�������
	
	PORT_Init(PORTA, PIN6, FUNMUX0_UART2_RXD, 1);	//GPIOA.6����ΪUART2��������
	
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
	UART_initStruct.TimeoutIEn = 0;
	
	UART_initStruct.Baudrate = 57600;
 	UART_Init(UART1, &UART_initStruct);
	UART_Open(UART1);
	
	UART_initStruct.Baudrate = 9600;
 	UART_Init(UART2, &UART_initStruct);
	UART_Open(UART2);
   	
	/* ����˵��
	*  ��UART1_TX�����ӵ�UART2_RX��UART1������57600��UART2������9600
	*  ��UART1����0xF8��UART2ͨ������0xF8�Զ�����������ʽ��Լ��Ĳ�������Ϊ57600
	****************************************************************************/
	while(1==1)
	{		
		UART_ABRStart(UART2, 0xF8);
		
		while((res = UART_ABRIsDone(UART2)) == 0)
		{
			UART_WriteByte(UART1, 0xF8);
			while(UART_IsTXBusy(UART1));
		}
		
		if(res == UART_ABR_RES_OK) printf("AutoBaud OK\r\n");
		else                       printf("AutoBaud Error\r\n");
		
		printf("UART1 Baud Reg: %d\r\n", UART_GetBaudrate(UART1));
		printf("UART2 Baud Reg: %d\r\n", UART_GetBaudrate(UART2));
		
		for(i = 0; i < 1000000; i++);
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
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
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
