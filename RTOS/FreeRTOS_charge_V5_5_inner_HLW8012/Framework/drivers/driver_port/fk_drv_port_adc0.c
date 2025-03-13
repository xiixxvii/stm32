
/*----------------------------------------------------------------------------------*/

#include "framework.h"
#if 0
/*----------------------------------------------------------------------------------*/
static void fk_drv_adc0_Init(void);															/*��ʼ��*/
static void fk_drv_adc0_Open(void);															/*����*/
static void fk_drv_adc0_Close(void);														/*�ر�*/
static void fk_drv_adc0_Tigger(void);														/*����һ��ת��ͨ��*/
static void fk_drv_adc0_SetIntRxFunc(int Ch,IntFunc2 func);			/*�����жϵ���*/
static void* fk_drv_adc0_GetConfigInfo(void);										/*��ȡ������Ϣ*/
static void fk_drv_adc0_ReInit(void *Info);											/*���³�ʼ��*/

/*----------------------------------------------------------------------------------*/

fk_drv_adc0_t DrvAdc0 ={
	.Init		= fk_drv_adc0_Init,
	.ReInit	= fk_drv_adc0_ReInit,
	.Open		= fk_drv_adc0_Open,
	.Close	= fk_drv_adc0_Close,
	.Tigger = fk_drv_adc0_Tigger,
	.SetIntRxFunc 	= fk_drv_adc0_SetIntRxFunc,
	.GetConfigInfo 	= fk_drv_adc0_GetConfigInfo,
};
ADC_InitStructure ADC_initStruct;
/*�жϵ���*/
static void IntError_0(unsigned int Dat){printf("rec = %c\r\n",Dat);}
static IntFunc2 thisDrvIntFunc[ADC0_UseCh]={IntError_0};


/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Init(void)
{
	
	/*��ʼ�����������ΪADC��������*/
	PORT_Init(PORTE, PIN4,  PORTE_PIN4_ADC0_IN0,  0);		//PE.4  => ADC0.CH0
//	PORT_Init(PORTA, PIN15, PORTA_PIN15_ADC0_IN1, 0);		//PA.15 => ADC0.CH1
//	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_IN2, 0);		//PA.14 => ADC0.CH2
//	PORT_Init(PORTA, PIN13, PORTA_PIN13_ADC0_IN3, 0);		//PA.13 => ADC0.CH3
//	PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC0_IN4, 0);		//PA.12 => ADC0.CH4
//	PORT_Init(PORTC, PIN7,  PORTC_PIN7_ADC0_IN5,  0);		//PC.7  => ADC0.CH5
//	PORT_Init(PORTC, PIN6,  PORTC_PIN6_ADC0_IN6,  0);		//PC.6  => ADC0.CH6
	
	/*����ADC0����ʱ��*/
	ADC_initStruct.clk_src 	= ADC_CLKSRC_HRC_DIV8;
	
	/*��ͨ��Щͨ��?��ʹ�� ��λ�� ͬʱʹ�ܶ��ͨ�� */
	ADC_initStruct.channels = ADC_CH0;				//|ADC_CH1|ADC_CH2|ADC_CH3
	
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;	//ת������
	ADC_initStruct.trig_src = ADC_TRIGGER_SW;		//�������
	ADC_initStruct.Continue = 0;								//������ģʽ��������ģʽ
	ADC_initStruct.EOC_IEn 	= ADC_CH0;					//ת�������ж�	|ADC_CH1|ADC_CH2|ADC_CH3
	ADC_initStruct.OVF_IEn 	= 0;								//����ж�
	ADC_Init(ADC0, &ADC_initStruct);						//����ADC
}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Open(void)		{ ADC_Open(ADC0);}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Close(void)	{ ADC_Close(ADC0);}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_Tigger(void)	{ ADC_Start(ADC0);}
/*----------------------------------------------------------------------------------*/
static	void fk_drv_adc0_SetIntRxFunc(int Ch,IntFunc2 func)
{
	thisDrvIntFunc[Ch] = func;
}
/*----------------------------------------------------------------------------------*/
static	void* fk_drv_adc0_GetConfigInfo(void)
{
	ADC_InitStructure* iInfo = &ADC_initStruct;
	return (void*)iInfo;
}

/*----------------------------------------------------------------------------------*/

static void fk_drv_adc0_ReInit(void *Info)
{
	ADC_Init(ADC0, Info);						//����ADC
}

/*----------------------------------------------------------------------------------*/
//�жϷ�����
void GPIOC4_ADC0_Handler(void)
{
	unsigned int Val = 0;
	/*�ȿ�����ʲôͨ���������ж�*/				
	if( ADC_IntEOCStat(ADC0,ADC_CH0) )		
	{
		Val = ADC_Read(ADC0, ADC_CH0);			//����ֵ
		//printf("Val = %d \r\n",Val);
		thisDrvIntFunc[0](Val);							//���ݴ�����
		ADC_IntEOCClr(ADC0,ADC_CH0);				//���ж�
	}
	
	//ADC_Start(ADC0);
}

/*----------------------------------------------------------------------------------*/
#endif








