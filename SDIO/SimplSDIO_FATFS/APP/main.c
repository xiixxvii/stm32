#include <string.h>
#include "SWM320.h"

#include "ff.h"

FATFS fatfs;

char *str = "Hi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\n"
			"Hi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\n"
			"Hi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\n"
			"Hi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\n"
			"Hi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\n"
			"Hi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\nHi from Synwit!\n";
char buf[1024] = {0};

void SerialInit(void);

int main(void)
{
	uint32_t len;
	FRESULT  res;
	FIL filw, filr;
	
	SystemInit();
	
	SerialInit();

	res = f_mount(&fatfs, "sd:", 1);
	if(res != FR_OK)
	{
		printf("sdcard init fail!\r\n");
		goto loop_forever;
	}
	
    res = f_open(&filw, "sd:test.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		printf("create file fail!\r\n");
		goto loop_forever;
	}
	
	res = f_write(&filw, str, strlen(str), &len);
	if(res != FR_OK)
	{
		printf("write file fail!\r\n");
		goto loop_forever;
	}
	
	f_close(&filw);
	
	res = f_open(&filr, "sd:test.txt", FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		printf("open file fail!\r\n");
		goto loop_forever;
	}
	
	res = f_read(&filr, buf, sizeof(buf)-1, &len);
	if(res != FR_OK)
	{
		printf("read file fial!\r\n");
		goto loop_forever;
	}
	
	f_close(&filr);
	
	if(strcmp(buf, str) == 0)
	{
		printf("read string == write string\r\n");
	}
	else
	{
		printf("read string != write string\r\n");
	}
	
loop_forever:
	while(1==1)
	{
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
