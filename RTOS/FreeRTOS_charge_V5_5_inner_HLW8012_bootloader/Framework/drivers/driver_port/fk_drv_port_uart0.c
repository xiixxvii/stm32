
/*----------------------------------------------------------------------------------*/

#include "framework.h"
/*----------------------------------------------------------------------------------*/
static void Init(uint32_t baudrate);
static void Open(void);
static void Close(void);
static void Write(unsigned char Dat);
static void SetIntRxFunc(IntFunc1 func);
static void EnableIRQ( void );
static void DisableIRQ( void );

static drv_uart_port_t do_drvp_uart={
	.Init = Init,
	.Open = Open,
	.Close = Close,
	.Write = Write,
	.SetIntRxFunc = SetIntRxFunc,
	.EnableIRQ = EnableIRQ,
	.DisableIRQ = DisableIRQ

};

drv_uart_port_pt drvp_uart0 = &do_drvp_uart;

static void IntError(unsigned char Dat){printf("rec = %c\r\n",Dat);}
static IntFunc1 thisDrvIntFunc = IntError; 								//�жϵ��ú���ָ��
/*----------------------------------------------------------------------------------*/

static void Init(uint32_t baudrate)
{
	UART_InitStructure UART_initStruct;
	//(1)��ʹ�ô���֮ǰ����ʼ��IO�ڵ����Ź��ܡ�
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2����ΪUART0��������
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3����ΪUART0�������
 	
	//(2)����һ���ṹ����������������ã��������
	/*���ò�����---------------------------------------------------------*/
 	UART_initStruct.Baudrate = baudrate;
	/*��������λ��------------------------------------------------------*/
	UART_initStruct.DataBits = UART_DATA_8BIT;
	/*������żУ��------------------------------------------------------*/
	UART_initStruct.Parity = UART_PARITY_NONE;
	/*����ֹͣλ----------------------------------------------------------*/
	UART_initStruct.StopBits = UART_STOP_1BIT;
	/*���ý���FIFO��λ������ֵ----------------------------------*/
	UART_initStruct.RXThreshold = 7;
	UART_initStruct.RXThresholdIEn = 1;
	/*���÷���FIFO��λ������ֵ----------------------------------*/
	UART_initStruct.TXThreshold = 0;
	UART_initStruct.TXThresholdIEn = 0;
	/*���ý��ճ�ʱʱ�� ��λ:��(�ַ�����)---------------------*/
	UART_initStruct.TimeoutTime = 250;
	/*�Ƿ�ʹ���ַ����ճ�ʱ�ж�-----------------------------------*/
	UART_initStruct.TimeoutIEn = 1;
	//(3)ʹ�����ú��������������õ�������Ч
 	UART_Init(UART0, &UART_initStruct);
}
/*----------------------------------------------------------------------------------*/

static void Open(void)		{UART_Open(UART0);}
static void Close(void)		{UART_Close(UART0);}
static void Write(unsigned char Dat)	
{ 
	while(1)
	{
		if( 0 == UART_IsTXBusy(UART0) )
		{
			break;
		}
	}	
	UART_WriteByte(UART0,Dat);
}

/*----------------------------------------------------------------------------------*/
static void SetIntRxFunc(IntFunc1 func){thisDrvIntFunc = func;}
/*----------------------------------------------------------------------------------*/
static void EnableIRQ( void )
{
	NVIC_EnableIRQ(UART0_IRQn);
}
static void DisableIRQ( void )
{
	NVIC_DisableIRQ(UART0_IRQn);
}
/*----------------------------------------------------------------------------------*/
void UART0_Handler(void)
{
	uint32_t chr;
#ifdef old_sdk
	/*---------------------------------------------------------------*/
	//���������ж�:���ո�λ�ж�/���ճ�ʱ�ж�
//	if(UART_INTRXThresholdStat(UART0) || UART_INTTimeoutStat(UART0)){
//		while(UART_IsRXFIFOEmpty(UART0) == 0)	{//������˵��RX FIFO�ǿ�
//			if(UART_ReadByte(UART0, &chr) == 0)	{thisDrvIntFunc(chr);}
//		}
//	}
#else	
	
	if(UART_INTStat(UART0, UART_IT_RX_THR | UART_IT_RX_TOUT))
	{
		while(UART_IsRXFIFOEmpty(UART0) == 0)
		{
			if(UART_ReadByte(UART0, &chr) == 0)
			{
				{thisDrvIntFunc(chr);}
			}
		}
	}
#endif
}


/*----------------------------------------------------------------------------------*/

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
/*----------------------------------------------------------------------------------*/


