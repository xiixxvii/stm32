#include "SWM320.h"

int main(void)
{	
	SystemInit();
	
	//(1)��ʼ��IO��Ϊ�������
	GPIO_Init(GPIOB, PIN12, 0, 1, 0);	

	//(2)��IO������͵�ƽ������״̬��
	GPIO_ClrBit(GPIOB, PIN12);	
	
	while(1==1)
	{
		
	}
}
