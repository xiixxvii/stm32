#include "SWM320.h"

uint16_t *TXBuff = 0; 
uint16_t TXCount = 0;
uint16_t TXIndex = 0;

void SerialInit(void);
void SPI_Master_Send(uint16_t buff[], uint16_t cnt);

int main(void)
{	
	uint32_t i;
	SPI_InitStructure SPI_initStruct;
	uint16_t txbuff[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
	
	SystemInit();
	
	SerialInit();	
	
	GPIO_Init(GPIOA, PIN3, 1, 0, 0);		//用作中断进入指示信号
	
	GPIO_Init(GPIOA, PIN12, 1, 0, 0);
#define SPI0_CS_Low()	GPIO_ClrBit(GPIOA, PIN12)
#define SPI0_CS_High()	GPIO_SetBit(GPIOA, PIN12)
	
	PORT_Init(PORTA, PIN9,  FUNMUX1_SPI0_SCLK, 0);
	PORT_Init(PORTA, PIN10, FUNMUX0_SPI0_MOSI, 0);
	PORT_Init(PORTA, PIN11, FUNMUX1_SPI0_MISO, 1);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_128;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXHFullIEn = 0;
	SPI_initStruct.TXEmptyIEn = 0;
	SPI_initStruct.TXCompleteIEn = 1;
	SPI_Init(SPI0, &SPI_initStruct);
	
	SPI_Open(SPI0);
	
	while(1==1)
	{
		SPI_Master_Send(txbuff, 16);
		
		for(i = 0; i < SystemCoreClock/100; i++) __NOP();
	}
}


void SPI_Master_Send(uint16_t buff[], uint16_t cnt)
{	
	TXBuff = buff;
	TXCount = cnt;
	TXIndex = 0;
	
	SPI0_CS_Low();
	
	SPI_INTEn(SPI0, SPI_IT_TX_EMPTY);
	SPI_INTEn(SPI0, SPI_IT_TX_DONE);
}

void SPI0_Handler(void)
{		
	if(SPI_INTStat(SPI0, SPI_IT_TX_DONE))
	{
		SPI_INTClr(SPI0, SPI_IT_TX_DONE);
		
		if(TXIndex == TXCount)	// 要发送的数据已全部填入SPI TX FIFO
		{
			SPI0_CS_High();
			SPI_INTDis(SPI0, SPI_IT_TX_DONE);
		}
	}
	
	if(SPI_INTStat(SPI0, SPI_IT_TX_EMPTY))
	{
		while(SPI_IsTXFull(SPI0) == 0)
		{
			if(TXIndex < TXCount)
			{
				SPI_Write(SPI0, TXBuff[TXIndex++]);
			}
			else
			{
				break;
			}
		}
		
		if(TXIndex == TXCount)
		{
			SPI_INTDis(SPI0, SPI_IT_TX_EMPTY);
		}
		
		SPI0->IF = SPI_IF_TFE_Msk;	//清除中断标志，必须在填充TX FIFO后清中断标志
	}
	
	GPIO_InvBit(GPIOA, PIN3);
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
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
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
