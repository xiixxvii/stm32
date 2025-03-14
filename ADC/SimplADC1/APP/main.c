#include "SWM320.h"

void SerialInit(void);

int main(void)
{
	uint32_t i;
	ADC_InitStructure ADC_initStruct;
	
	SystemInit();
	
	SerialInit();   	
	
	PORT_Init(PORTC, PIN7, PORTC_PIN7_ADC1_IN0, 0);		//PC.7 => ADC1.CH0
	PORT_Init(PORTC, PIN6, PORTC_PIN6_ADC1_IN1, 0);		//PC.6 => ADC1.CH1
	PORT_Init(PORTC, PIN5, PORTC_PIN5_ADC1_IN2, 0);		//PC.5 => ADC1.CH2
	PORT_Init(PORTC, PIN4, PORTC_PIN4_ADC1_IN3, 0);		//PC.4 => ADC1.CH3
	PORT_Init(PORTN, PIN0, PORTN_PIN0_ADC1_IN4, 0);		//PN.0 => ADC1.CH4
	PORT_Init(PORTN, PIN1, PORTN_PIN1_ADC1_IN5, 0);		//PN.1 => ADC1.CH5
	PORT_Init(PORTN, PIN2, PORTN_PIN2_ADC1_IN6, 0);		//PN.2 => ADC1.CH6
	
	/* 使用 ADC1 时，系统时钟最好选 SYS_CLK_PLL，不要选 SYS_CLK_40MHz
	   因为 ADC1 只能使用VCO时钟，若系统时钟不是 PLL，ADC_Init() 中会调用 PLLInit()，
	   若 SYS_PLL_SRC 值为 SYS_CLK_20MHz，则系统时钟会被从 40MHz 切换回 20MHz，导致
	   SystemCoreClock 值与实际系统时钟不符，串口打印乱码
	*/
	ADC_initStruct.clk_src = ADC_CLKSRC_VCO_DIV64;		//注意：ADC1只能使用VCO时钟，不能使用ADC_CLKSRC_HRC
	ADC_initStruct.clk_div = 25;
	ADC_initStruct.pga_ref = PGA_REF_INTERNAL;
	ADC_initStruct.channels = ADC_CH0;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGSRC_SW;
	ADC_initStruct.Continue = 0;						//非连续模式，即单次模式
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_initStruct.HFULL_IEn = 0;
	ADC_initStruct.FULL_IEn = 0;
	ADC_Init(ADC1, &ADC_initStruct);					//配置ADC
	
	ADC_Open(ADC1);										//使能ADC
	
	while(1==1)
	{
		ADC_Start(ADC1);
		while((ADC1->CH[0].STAT & 0x01) == 0);
		printf("%4d,", ADC_Read(ADC1, ADC_CH0));
		
		for(i = 0; i < SystemCoreClock/1000; i++) __NOP();
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
