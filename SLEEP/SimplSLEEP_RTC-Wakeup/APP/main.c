#include "SWM320.h"

void SerialInit(void);
void RTC_Config(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	RTC_Config();
	SYS->RTCWKEN |= 1;								//开启RTC唤醒
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0);				//输出，接LED
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);					//点亮LED
		for(i = 0; i < 10000000; i++) __NOP();
		GPIO_ClrBit(GPIOA, PIN5);					//熄灭LED
		
		__disable_irq();
		
		switchCLK_32KHz();
		SYS->HRCCR = (1 << SYS_HRCCR_OFF_Pos);		//关闭HRC、降低功耗
		
		SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);	//进入睡眠模式
		
		for(i = 0; i < 10; i++) __NOP();
		
		switchCLK_40MHz();	//切换到休眠前的时钟
		
		__enable_irq();
	}
}


void RTC_Config(void)
{
	RTC_InitStructure RTC_initStruct;
	RTC_AlarmStructure alarmStruct;
	
	RTC_initStruct.Year = 2018;
	RTC_initStruct.Month = 3;
	RTC_initStruct.Date = 23;
	RTC_initStruct.Hour = 10;
	RTC_initStruct.Minute = 5;
	RTC_initStruct.Second = 5;
	RTC_initStruct.SecondIEn = 0;
	RTC_initStruct.MinuteIEn = 0;
	RTC_Init(RTC, &RTC_initStruct);
	
	RTC_Start(RTC);
	
	alarmStruct.Days = RTC_SUN | RTC_MON | RTC_TUE | RTC_WED | RTC_THU | RTC_FRI | RTC_SAT;
	alarmStruct.Hour = RTC_initStruct.Hour;
	alarmStruct.Minute = RTC_initStruct.Minute;
	alarmStruct.Second = RTC_initStruct.Second + 5;
	alarmStruct.AlarmIEn = 1;
	
	RTC_AlarmSetup(RTC, &alarmStruct);
}

void RTC_Handler(void)
{
	RTC_DateTime dateTime;
	RTC_AlarmStructure alarmStruct;
	
	if(RTC_IntAlarmStat(RTC))
	{
		RTC_IntAlarmClr(RTC);
		
		RTC_GetDateTime(RTC, &dateTime);
		printf("Now Time: %02d : %02d\r\n", dateTime.Minute, dateTime.Second);
		
		alarmStruct.Days = RTC_SUN | RTC_MON | RTC_TUE | RTC_WED | RTC_THU | RTC_FRI | RTC_SAT;
		alarmStruct.Hour = dateTime.Hour;
		alarmStruct.Minute = dateTime.Minute;
		alarmStruct.Second = dateTime.Second + 5;
		alarmStruct.AlarmIEn = 1;
		
		if(alarmStruct.Second > 59)
		{
			alarmStruct.Second -= 60;
			alarmStruct.Minute += 1;
		}
		
		if(alarmStruct.Minute > 59)
		{
			alarmStruct.Minute = 0;
			alarmStruct.Hour += 1;
		}
		
		if(alarmStruct.Hour > 23)
		{
			alarmStruct.Hour = 0;
		}
		
		RTC_AlarmSetup(RTC, &alarmStruct);
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN2, FUNMUX0_UART0_RXD, 1);	//GPIOA.2配置为UART0输入引脚
	PORT_Init(PORTA, PIN3, FUNMUX1_UART0_TXD, 0);	//GPIOA.3配置为UART0输出引脚
 	
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
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
