
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


drv_uart_port_pt drvp_uart2 = &do_drvp_uart;

static void IntError(unsigned char Dat){printf("rec = %c\r\n",Dat);}
static IntFunc1 thisDrvIntFunc = IntError; 								//�жϵ��ú���ָ��
/*----------------------------------------------------------------------------------*/

static void Init(uint32_t baudrate)
{
	UART_InitStructure UART_initStruct;
	//(1)��ʹ�ô���֮ǰ����ʼ��IO�ڵ����Ź��ܡ�
	GPIO_Init( GPIOM, PIN4,0,0,1 );	//

	PORT_Init( PORTM, PIN4, FUNMUX0_UART2_RXD, 1);	//
 	//(2)����һ���ṹ����������������ã��������
	/*���ò�����---------------------------------------------------------*/
 	UART_initStruct.Baudrate = baudrate;
	/*��������λ��------------------------------------------------------*/
	UART_initStruct.DataBits = UART_DATA_8BIT;
	/*������żУ��------------------------------------------------------*/
	UART_initStruct.Parity = UART_PARITY_EVEN;
	/*����ֹͣλ----------------------------------------------------------*/
	UART_initStruct.StopBits = UART_STOP_1BIT;
	/*���ý���FIFO��λ������ֵ----------------------------------*/
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 1;
	/*���÷���FIFO��λ������ֵ----------------------------------*/
	UART_initStruct.TXThreshold = 0;
	UART_initStruct.TXThresholdIEn = 0;
	/*���ý��ճ�ʱʱ�� ��λ:��(�ַ�����)---------------------*/
	UART_initStruct.TimeoutTime = 250;
	/*�Ƿ�ʹ���ַ����ճ�ʱ�ж�-----------------------------------*/
	UART_initStruct.TimeoutIEn = 1;
	
	//(3)ʹ�����ú��������������õ�������Ч
 	UART_Init(UART2, &UART_initStruct);
}
/*----------------------------------------------------------------------------------*/

static void Open(void)		{UART_Open(UART2);	}
static void Close(void)		{UART_Close(UART2);	}

static void Write( unsigned char Dat )
{ 
	while(1)
	{
		if( 0 == UART_IsTXBusy(UART2) )
		{
			break;
		}
	}	
	UART_WriteByte(UART2,Dat);
}

/*----------------------------------------------------------------------------------*/
static void SetIntRxFunc(IntFunc1 func){thisDrvIntFunc = func;}
/*----------------------------------------------------------------------------------*/
static void EnableIRQ( void )
{
	NVIC_EnableIRQ(UART2_IRQn);
}

static void DisableIRQ( void )
{
	NVIC_DisableIRQ(UART2_IRQn);
}

/*----------------------------------------------------------------------------------*/
void UART2_Handler(void)
{
	uint32_t chr;
#ifdef old_sdk
	/*---------------------------------------------------------------*/
	//���������ж�:���ո�λ�ж�/���ճ�ʱ�ж�
//	if(UART_INTRXThresholdStat(UART2) || UART_INTTimeoutStat(UART2)){
//		while(UART_IsRXFIFOEmpty(UART2) == 0)	{//������˵��RX FIFO�ǿ�
//			if(UART_ReadByte(UART2, &chr) == 0)	{thisDrvIntFunc(chr);}
//		}
//	}
#else	
	
	if(UART_INTStat(UART2, UART_IT_RX_THR | UART_IT_RX_TOUT))
	{
		while(UART_IsRXFIFOEmpty(UART2) == 0)
		{
			if(UART_ReadByte(UART2, &chr) == 0)
			{
				{thisDrvIntFunc(chr);}
			}
		}
	}
#endif
}


/*----------------------------------------------------------------------------------*/

