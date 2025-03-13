#include "SWM320.h"

void SerialInit(void);

int main(void)
{	
	uint32_t n, i;
	uint32_t rxdata, txdata = 0x23;
	SPI_InitStructure SPI_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN12, 1, 0, 0);
#define SPI1_CS_Low()	GPIO_ClrBit(GPIOA, PIN12)
#define SPI1_CS_High()	GPIO_SetBit(GPIOA, PIN12)
	
	PORT_Init(PORTA, PIN9,  FUNMUX1_SPI1_SCLK, 0);
	PORT_Init(PORTA, PIN10, FUNMUX0_SPI1_MOSI, 0);
	PORT_Init(PORTA, PIN11, FUNMUX1_SPI1_MISO, 1);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXHFullIEn = 0;
	SPI_initStruct.TXEmptyIEn = 0;
	SPI_initStruct.TXCompleteIEn = 0;
	SPI_Init(SPI1, &SPI_initStruct);
	SPI_Open(SPI1);
	
	while(1==1)
	{
		SPI1_CS_Low();
		for(n = 0; n < 10; n++)
		{
			rxdata = SPI_ReadWrite(SPI1, txdata);
			txdata = rxdata + 1;		//��SPI0_MOSI��SPI0_MISO�̽ӣ���ɿ�����ӡ��ֵ��һ����
			
			printf("rxdata: 0x%X\r\n", rxdata);
			
			for(i = 0; i < SystemCoreClock/50; i++);
		}
		SPI1_CS_High();
		
		for(i = 0; i < SystemCoreClock/10; i++);
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
