#include "SWM320.h"

int main(void)
{	
	SystemInit();
	
	//(1)初始化IO口为输出功能
	GPIO_Init(GPIOB, PIN12, 0, 1, 0);	

	//(2)让IO口输出低电平，点亮状态灯
	GPIO_ClrBit(GPIOB, PIN12);	
	
	while(1==1)
	{
		
	}
}
