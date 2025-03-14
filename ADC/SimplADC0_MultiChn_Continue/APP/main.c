#include "SWM320.h"

void SerialInit(void);

uint16_t Buffer[4][500] = {0};

int main(void)
{
	uint32_t n, i;
	ADC_InitStructure ADC_initStruct;
	
	SystemInit();
	
	SerialInit();	
	
	PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC0_IN4, 0);	//PA.12 => ADC.CH4
	PORT_Init(PORTA, PIN11, PORTA_PIN11_ADC0_IN5, 0);	//PA.11 => ADC.CH5
	PORT_Init(PORTA, PIN10, PORTA_PIN10_ADC0_IN6, 0);	//PA.10 => ADC.CH6
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_ADC0_IN7,  0);	//PA.9  => ADC.CH7
	
	ADC_initStruct.clk_src = ADC_CLKSRC_VCO_DIV64;
	ADC_initStruct.clk_div = 25;
	ADC_initStruct.pga_ref = PGA_REF_INTERNAL;
	ADC_initStruct.channels = ADC_CH0 | ADC_CH1 | ADC_CH2 | ADC_CH3;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGSRC_SW;
	ADC_initStruct.Continue = 1;						//非连续模式，即单次模式
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_initStruct.HFULL_IEn = 0;
	ADC_initStruct.FULL_IEn = 0;
	ADC_Init(ADC0, &ADC_initStruct);					//配置ADC
		
	ADC_Open(ADC0);										//使能ADC
	
	while(1==1)
	{
		ADC_Start(ADC0);
		
		for(n = 0; n < 125; n++)
		{
			while((ADC0->CH[3].STAT & 0x04) == 0);
			for(i = 0; i < 4; i++)
			{
				Buffer[0][4*n+i] = ADC0->CH[0].DATA;
				Buffer[1][4*n+i] = ADC0->CH[1].DATA;
				Buffer[2][4*n+i] = ADC0->CH[2].DATA;
				Buffer[3][4*n+i] = ADC0->CH[3].DATA;
			}
		}
		
		ADC_Stop(ADC0);
		
		printf("\r\nCH0:\r\n");
		for(i = 0; i < 500; i++)
			printf("%d,", Buffer[0][i]);
		printf("\r\nCH1:\r\n");
		for(i = 0; i < 500; i++)
			printf("%d,", Buffer[1][i]);
		printf("\r\nCH2:\r\n");
		for(i = 0; i < 500; i++)
			printf("%d,", Buffer[2][i]);
		printf("\r\nCH3:\r\n");
		for(i = 0; i < 500; i++)
			printf("%d,", Buffer[3][i]);
		printf("\r\n\r\n");
				
		for(i = 0; i < 8; i++)
		{
			Buffer[0][i] = ADC0->CH[0].DATA;
			Buffer[1][i] = ADC0->CH[1].DATA;
			Buffer[2][i] = ADC0->CH[2].DATA;
			Buffer[3][i] = ADC0->CH[3].DATA;
		}
	}
}

void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);		//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);		//GPIOA.3配置为UART0输出引脚
 	
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
