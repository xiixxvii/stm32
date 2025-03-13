#include "SWM320.h"

uint16_t Buffer[1000] = {0};

void SerialInit(void);

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
	
	ADC_initStruct.clk_src = ADC_CLKSRC_HRC;
	ADC_initStruct.clk_div = 10;
	ADC_initStruct.pga_ref = PGA_REF_INTERNAL;
	ADC_initStruct.channels = ADC_CH0;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGSRC_SW;
	ADC_initStruct.Continue = 1;						//������ģʽ��������ģʽ
	ADC_initStruct.EOC_IEn = 0;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_initStruct.HFULL_IEn = 0;
	ADC_initStruct.FULL_IEn = 0;
	ADC_Init(ADC0, &ADC_initStruct);					//����ADC
		
	ADC_Open(ADC0);										//ʹ��ADC
	
	while(1==1)
	{
		ADC_Start(ADC0);
		
		for(n = 0; n < 1000/4; n++)
		{
			while((ADC0->CH[0].STAT & 0x04) == 0);
			for(i = 0; i < 4; i++)
			{
				Buffer[4*n+i] = ADC0->CH[0].DATA;
			}
		}
		
		ADC_Stop(ADC0);
		
		for(i = 0; i < 1000; i++)
			printf("%d,", Buffer[i]);
	}
}

void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);		//GPIOA.2����ΪUART0��������
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);		//GPIOA.3����ΪUART0�������
 	
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
