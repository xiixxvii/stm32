#include "SWM320.h"

/* ע�⣺
	1������STOP��ֻ��ͨ��Wake-up���Ż��ѣ����ܰ���λ������λоƬ
	2����STOPģʽWake-up������ͷ��ʼ����ִ�У���Backup�������ݿ��Ա���
	3��Wake-up������PB12���½��ػ��ѣ������ڼ���Ҫ���ָߵ�ƽ
*/

void SerialInit(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	SYS->CLKEN |= (1 << SYS_CLKEN_RTCBKP_Pos);		//Backup����Ҫ������ʱ��
	printf("RTCBKP[0]: %08X\r\n", SYS->RTCBKP[0]++);
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);				//����� ��LED
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);					//����LED
		for(i = 0; i < 10000000; i++) __NOP();
		GPIO_ClrBit(GPIOA, PIN5);					//Ϩ��LED
		
		switchCLK_32KHz();
		SYS->HRCCR = (1 << SYS_HRCCR_OFF_Pos);		//�ر�HRC�����͹���
		
		SYS->SLEEP |= (1 << SYS_SLEEP_DEEP_Pos);	//����STOPģʽ
		
		while(1) __NOP();	//���Ѻ��ͷ��ʼ����ִ��
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
