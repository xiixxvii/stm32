#include "SWM320.h"

volatile uint32_t TX_Times = 0;

void SerialInit(void);

int main(void)
{
	uint32_t i;
	CAN_InitStructure CAN_initStruct;
	uint8_t tx_data[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTA, PIN4, FUNMUX0_CAN_RX, 1);	//GPIOA.4����ΪCAN��������
	PORT_Init(PORTA, PIN5, FUNMUX1_CAN_TX, 0);	//GPIOA.5����ΪCAN�������
	
	CAN_initStruct.Mode = CAN_MODE_SELFTEST;	//�Բ�ģʽ��TX��RXҪ������һ��
	CAN_initStruct.CAN_BS1 = CAN_BS1_5tq;
	CAN_initStruct.CAN_BS2 = CAN_BS2_4tq;
	CAN_initStruct.CAN_SJW = CAN_SJW_2tq;
	CAN_initStruct.Baudrate = 100000;
	CAN_initStruct.FilterMode = CAN_FILTER_32b;
   	CAN_initStruct.FilterMask32b = 0xFFFFFFFF;
	CAN_initStruct.FilterCheck32b = 0xFFFFFFFF;
	CAN_initStruct.RXNotEmptyIEn = 0;
	CAN_initStruct.RXOverflowIEn = 0;
	CAN_initStruct.ArbitrLostIEn = 0;
	CAN_initStruct.ErrPassiveIEn = 0;
	CAN_Init(CAN, &CAN_initStruct);
	
	CAN_Open(CAN);
	
	CAN_INTEn(CAN, CAN_IT_TX_EMPTY);
	NVIC_EnableIRQ(CAN_IRQn);
	
	while(1==1)
	{
		TX_Times = 0;
		CAN_Transmit(CAN, CAN_FRAME_STD, 0x3FF, tx_data, 8, 0);
		
		for(i = 0; i < SystemCoreClock/4; i++);
	}
}


void CAN_Handler(void)
{
	uint8_t tx_data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	uint32_t int_sr = CAN_INTStat(CAN);
	
	if(int_sr & CAN_IF_TXBR_Msk)
	{
		if(TX_Times < 4)
		{
			TX_Times++;
			
			CAN_Transmit(CAN, CAN_FRAME_STD, 0x3FF, tx_data, 8, 0);
		}
		else
		{
			volatile uint32_t tmp = CAN->SR;	// �������һ�� CAN ��������ַ��������ж�
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
