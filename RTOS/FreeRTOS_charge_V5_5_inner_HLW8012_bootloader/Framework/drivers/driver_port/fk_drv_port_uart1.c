
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


drv_uart_port_pt drvp_uart1 = &do_drvp_uart;

static void IntError(unsigned char Dat){printf("rec = %c\r\n",Dat);}
static IntFunc1 thisDrvIntFunc = IntError; 								//�жϵ��ú���ָ��
/*----------------------------------------------------------------------------------*/

static void Init(uint32_t baudrate)
{
	UART_InitStructure UART_initStruct;
	//(1)��ʹ�ô���֮ǰ����ʼ��IO�ڵ����Ź��ܡ�
#if ( BoardVision == BoardV5_4)

	PORT_Init(PORTN, PIN6, FUNMUX0_UART1_RXD, 1);	//
	PORT_Init(PORTN, PIN5, FUNMUX1_UART1_TXD, 0);	//
#endif
	
#if ( BoardVision == BoardV5_5)

	PORT_Init(PORTP, PIN6, FUNMUX0_UART1_RXD, 1);	//
	PORT_Init(PORTP, PIN7, FUNMUX1_UART1_TXD, 0);	//
#endif
	
	
 	//(2)����һ���ṹ����������������ã��������
	/*���ò�����---------------------------------------------------------*/
 	UART_initStruct.Baudrate = baudrate;
	/*��������λ��------------------------------------------------------*/
	UART_initStruct.DataBits = UART_DATA_8BIT;
	/*������żУ��------------------------------------------------------*/
	UART_initStruct.Parity = UART_PARITY_NONE;//UART_PARITY_NONE;
	/*����ֹͣλ----------------------------------------------------------*/
	UART_initStruct.StopBits = UART_STOP_1BIT;
	/*���ý���FIFO��λ������ֵ----------------------------------*/
	UART_initStruct.RXThreshold = 0;
	UART_initStruct.RXThresholdIEn = 1;
	/*���÷���FIFO��λ������ֵ----------------------------------*/
	UART_initStruct.TXThreshold = 0;
	UART_initStruct.TXThresholdIEn = 0;
	/*���ý��ճ�ʱʱ�� ��λ:��(�ַ�����)---------------------*/
	UART_initStruct.TimeoutTime = 100;
	/*�Ƿ�ʹ���ַ����ճ�ʱ�ж�-----------------------------------*/
	UART_initStruct.TimeoutIEn = 1;
	
	//(3)ʹ�����ú��������������õ�������Ч
 	UART_Init(UART1, &UART_initStruct);
}
/*----------------------------------------------------------------------------------*/

static void Open(void)			{UART_Open(UART1);}
static void Close(void)		{UART_Close(UART1);}
static void Write(unsigned char Dat)	
{ 
	while(1)
	{
		if( 0 == UART_IsTXBusy(UART1) )
		{
			break;
		}
	}	
	UART_WriteByte(UART1,Dat);
}

/*----------------------------------------------------------------------------------*/
static void SetIntRxFunc(IntFunc1 func){thisDrvIntFunc = func;}
/*----------------------------------------------------------------------------------*/
static void EnableIRQ( void )
{
	NVIC_EnableIRQ(UART1_IRQn);
}
static void DisableIRQ( void )
{
	NVIC_DisableIRQ(UART1_IRQn);
}

/*----------------------------------------------------------------------------------*/
void UART1_Handler(void)
{
	uint32_t chr;
#ifdef old_sdk
	/*---------------------------------------------------------------*/
	//���������ж�:���ո�λ�ж�/���ճ�ʱ�ж�
//	if(UART_INTRXThresholdStat(UART1) || UART_INTTimeoutStat(UART1)){
//		while(UART_IsRXFIFOEmpty(UART1) == 0)	{//������˵��RX FIFO�ǿ�
//			if(UART_ReadByte(UART1, &chr) == 0)	{thisDrvIntFunc(chr);}
//		}
//	}
#else	
	
	if(UART_INTStat(UART1, UART_IT_RX_THR | UART_IT_RX_TOUT))
	{
		while(UART_IsRXFIFOEmpty(UART1) == 0)
		{
			if(UART_ReadByte(UART1, &chr) == 0)
			{
				{thisDrvIntFunc(chr);}
			}
		}
	}
#endif

}


/*----------------------------------------------------------------------------------*/

