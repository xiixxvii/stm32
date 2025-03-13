#include "SWM320.h"


/* ע�⣺
 *	оƬ�� ISP��SWD ����Ĭ�Ͽ������������裬���������߹��ģ�������������߹��ģ�����ǰ��ر��������ŵ���������������
 */
 
 
void SerialInit(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN4, 0, 1, 0);				//���룬����ʹ�ܣ���KEY
	SYS->PAWKEN |= (1 << PIN4);						//����PA4���ŵ͵�ƽ����
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);				//�������LED
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);					//����LED
		for(i = 0; i < 10000000; i++);
		GPIO_ClrBit(GPIOA, PIN5);					//Ϩ��LED
		
		__disable_irq();
		
		switchCLK_32KHz();
		SYS->HRCCR = (1 << SYS_HRCCR_OFF_Pos);		//�ر�HRC�����͹���
		
		SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);	//����˯��ģʽ
	
		while((SYS->PAWKSR & (1 << PIN4)) == 0);	//�ȴ���������
		SYS->PAWKSR |= (1 << PIN4);					//�������״̬
		
		switchCLK_40MHz();	//�л�������ǰ��ʱ��
		
		__enable_irq();
		
		printf("SWM320 wakeup!\r\n");
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
