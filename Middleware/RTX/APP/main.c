#include "SWM320.h"

#include <RTL.h>

/* ע�⣺
*	Ϊ�˼򵥣���RX_Conf_CM.c����������Task����������Ȩ�ȼ�������Task��
* ����дĳЩ�ں˼Ĵ����������NVIC_DisableIRQ())ʱ�ͻᴥ��HardFault
************************************************************************/

os_mbx_declare(queueADC, 16);

__task void TaskINI(void);
__task void TaskADC(void);
__task void TaskPWM(void);
void SerialInit(void);

int main(void)
{ 	
 	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);	//����ָʾ�ź�
	
	os_sys_init(TaskINI);
}

__task void TaskINI(void)
{
	os_tsk_create(TaskADC, 1);
	os_tsk_create(TaskPWM, 2);
	
	os_mbx_init(&queueADC, sizeof(queueADC));
	
	os_tsk_delete(os_tsk_self());
}

/****************************************************************************************************************************************** 
* ��������:	TaskADC()
* ����˵��: ����ADC�ɼ�����
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
__task void TaskADC(void)
{
	ADC_InitStructure ADC_initStruct;
	
	PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC0_IN4, 0);	//PA.12 => ADC.CH4
	PORT_Init(PORTA, PIN11, PORTA_PIN11_ADC0_IN5, 0);	//PA.11 => ADC.CH5
	PORT_Init(PORTA, PIN10, PORTA_PIN10_ADC0_IN6, 0);	//PA.10 => ADC.CH6
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_ADC0_IN7,  0);	//PA.9  => ADC.CH7
	
	ADC_initStruct.clk_src = ADC_CLKSRC_VCO_DIV64;
	ADC_initStruct.clk_div = 25;
	ADC_initStruct.pga_ref = PGA_REF_INTERNAL;
	ADC_initStruct.channels = ADC_CH4;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_initStruct.trig_src = ADC_TRIGSRC_SW;
	ADC_initStruct.Continue = 0;						//������ģʽ��������ģʽ
	ADC_initStruct.EOC_IEn = ADC_CH4;	
	ADC_initStruct.OVF_IEn = 0;
	ADC_initStruct.HFULL_IEn = 0;
	ADC_initStruct.FULL_IEn = 0;
	ADC_Init(ADC0, &ADC_initStruct);					//����ADC
	
	ADC_Open(ADC0);										//ʹ��ADC
	
	while(1)
	{
		ADC_Start(ADC0);
		
		os_dly_wait(100);
	}
}

void ADC0_Handler(void)
{
	uint32_t val;
	
	ADC0->IF = (1 << ADC_IF_CH4EOC_Pos);
	
	val = ADC_Read(ADC0, ADC_CH4);
	
	isr_mbx_send(&queueADC, (void *)val);
	
	GPIO_InvBit(GPIOA, PIN5);
}

/****************************************************************************************************************************************** 
* ��������:	TaskPWM()
* ����˵��: �ȴ�ADCת�����������ADCת���������PWMռ�ձ�
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
__task void TaskPWM(void)
{
	PWM_InitStructure  PWM_initStruct;
	
	PWM_initStruct.clk_div = PWM_CLKDIV_8;		//F_PWM = 20M/8 = 2.5M
	
	PWM_initStruct.mode = PWM_MODE_INDEP;		//A·��B·�������					
	PWM_initStruct.cycleA = 10000;				//2.5M/10000 = 250Hz			
	PWM_initStruct.hdutyA =  2500;				//2500/10000 = 25%
	PWM_initStruct.initLevelA = 1;
	PWM_initStruct.cycleB = 10000;
	PWM_initStruct.hdutyB =  5000;				//5000/10000 = 50%
	PWM_initStruct.initLevelB = 1;
	PWM_initStruct.HEndAIEn = 0;
	PWM_initStruct.NCycleAIEn = 0;
	PWM_initStruct.HEndBIEn = 0;
	PWM_initStruct.NCycleBIEn = 0;
	
	PWM_Init(PWM0, &PWM_initStruct);
	
	PORT_Init(PORTA, PIN4, FUNMUX0_PWM0A_OUT, 0);
	PORT_Init(PORTA, PIN6, FUNMUX0_PWM0B_OUT, 0);
	
	PWM_Start(PWM0, 1, 1);
	
	while(1)
	{
		uint16_t duty;
		
		if(os_mbx_wait(queueADC, (void *)&duty, 10) != OS_R_TMO)
		{
			printf("%d,", duty);
			if(duty <  100) duty =  100;
			if(duty > 4000) duty = 4000;
			
			PWM_SetHDuty(PWM0, PWM_CH_A, 10000 * duty / 4096);
			PWM_SetHDuty(PWM0, PWM_CH_B, 10000 - PWM_GetHDuty(PWM0, PWM_CH_A));
		}
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
