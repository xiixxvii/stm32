#include "SWM320.h"

void SerialInit(void);
void PWMSignal(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	PWMSignal();	// ����PWM�����ź�
	
	PORT_Init(PORTA, PIN5, FUNMUX1_PULSE_IN, 1);
	Pulse_Init(PULSE_HIGH, 0);		//����PA5�����ϵĸ�������
	
	while(1==1)
	{
		Pulse_Start();
		while(Pulse_Done() == 0);
		printf("%d\r\n", TIMRG->PCVAL);
		
		for(i = 0; i < 5000000; i++) __NOP();
	}
}


void PWMSignal(void)
{
	PWM_InitStructure  PWM_initStruct;
	
	PWM_initStruct.clk_div = PWM_CLKDIV_8;		//F_PWM = 40M/8 = 5M
	
	PWM_initStruct.mode = PWM_MODE_INDEP;		//A·��B·�������					
	PWM_initStruct.cycleA = 50000;				//   5M/50000 = 100Hz			
	PWM_initStruct.hdutyA = 25000;				//25000/50000 = 50%
	PWM_initStruct.initLevelA = 1;
	PWM_initStruct.cycleB = 50000;
	PWM_initStruct.hdutyB = 20000;
	PWM_initStruct.initLevelB = 1;
	PWM_initStruct.HEndAIEn = 0;
	PWM_initStruct.NCycleAIEn = 0;
	PWM_initStruct.HEndBIEn = 0;
	PWM_initStruct.NCycleBIEn = 0;
	
	PWM_Init(PWM0, &PWM_initStruct);
	
	PORT_Init(PORTA, PIN4, FUNMUX0_PWM0A_OUT, 0);
	PORT_Init(PORTA, PIN6, FUNMUX0_PWM0B_OUT, 0);
	
	PWM_Start(PWM0, 1, 1);
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
