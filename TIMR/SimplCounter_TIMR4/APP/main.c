#include "SWM320.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);
		
	SYS->LRCCR &= ~(1 << SYS_LRCCR_OFF_Pos);	// ����32Kʱ��
	
	TIMR_Init(TIMR4, TIMR_MODE_COUNTER, 32, 1);	// ÿ��32�����ؽ����ж�
	
	TIMR_Start(TIMR4);
	
	while(1==1)
	{
	}
}

void TIMR4_Handler(void)
{
	TIMR_INTClr(TIMR4);
	
	GPIO_InvBit(GPIOA, PIN5);
}
