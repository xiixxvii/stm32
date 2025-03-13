#include "SWM320.h"

#include "ugui.h"


#define LCD_HDOT	480		// ˮƽ����
#define LCD_VDOT	272		// ��ֱ����
#define LCD_DIRH	1		// ˮƽ��ʾ��


/* ע�⣺
	�洢���Ĵ�����Ҫ����LCDˢ���������������ʾ�������Ӱ
	���ʹ��LCDʱ�ӷ�Ƶ����SRAMʱ�ӷ�Ƶ��2���������ɱ�֤SRAM�ڱ�CPUд���ͬʱ����Ȼ�ܹ�����LCD��ȡ�Ĵ���
*/


UG_GUI gui;


uint16_t *LCD_Buffer = (uint16_t *)SRAMM_BASE;

void _HW_DrawPoint(UG_S16 x, UG_S16 y, UG_COLOR c)
{	
#if LCD_DIRH
	LCD_Buffer[y * LCD_HDOT + x] = c;
#else
	LCD_Buffer[(LCD_VDOT - x) * LCD_HDOT + y] = c;
#endif
}


void SerialInit(void);
void MemoryInit(void);
void RGBLCDInit(void);

int main(void)
{
	uint32_t i,j;
	uint16_t color[3] = {C_GREEN, C_BLACK, C_BLUE};
	
	SystemInit();
	
	for(i = 0; i < 1000000; i++) __NOP();	// �ȴ���Ļ��ɸ�λ
	
	SerialInit();
	
	MemoryInit();
	
	RGBLCDInit();
	
	LCD->SRCADDR = (uint32_t)LCD_Buffer;
	
	LCD_Start(LCD);
	
#if LCD_DIRH
	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))_HW_DrawPoint, LCD_HDOT, LCD_VDOT);
#else
	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))_HW_DrawPoint, LCD_VDOT, LCD_HDOT);
#endif
		
 	UG_FillScreen(C_RED);
	
	UG_DrawLine(40, 50, 240, 50, C_GREEN);
	
	UG_DrawLine(40, 36, 40, 236, C_BLUE);
		
	UG_DrawCircle(200, 210, 50, C_GREEN);
	UG_DrawCircle(200, 210, 51, C_GREEN);
	UG_DrawCircle(200, 210, 52, C_GREEN);

	while(1==1)
	{		
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < SystemCoreClock/20; j++) __NOP();
			
			UG_FillFrame(60, 75, 200, 150, color[i]);
		}
	}
}


void RGBLCDInit(void)
{
	LCD_InitStructure LCD_initStruct;
	
	GPIO_Init(GPIOC, PIN2, 1, 0, 0);//�������
	GPIO_SetBit(GPIOC, PIN2);		//��������
	
	PORT->PORTN_SEL0 = 0xAAAAAAAA;	//GPION.0��15  LCD_DATA0��15
	PORT->PORTN_SEL1 = 0xAA;
	
	LCD_initStruct.HnPixel = LCD_HDOT;
	LCD_initStruct.VnPixel = LCD_VDOT;
	LCD_initStruct.Hfp = 5;
	LCD_initStruct.Hbp = 40;
	LCD_initStruct.Vfp = 8;
	LCD_initStruct.Vbp = 8;
	LCD_initStruct.ClkDiv = LCD_CLKDIV_16;	// ע���ֲ�����Ļ��ʱ��Ҫ��
	LCD_initStruct.ClkAlways = 0;
	LCD_initStruct.SamplEdge = LCD_SAMPLEDGE_FALL;
	LCD_initStruct.HsyncWidth = LCD_HSYNC_3DOTCLK;
	LCD_initStruct.IntEOTEn = 1;
	LCD_Init(LCD, &LCD_initStruct);
}

void LCD_Handler(void)
{
	LCD_INTClr(LCD);
	
	LCD_Start(LCD);
}


void MemoryInit(void)
{
	SRAM_InitStructure SRAM_InitStruct;
	
	PORT->PORTP_SEL0 = 0xAAAAAAAA;		//PP0-23 => ADDR0-23
	PORT->PORTP_SEL1 = 0x0000AAAA;
	
	PORT->PORTM_SEL0 = 0xAAAAAAAA;		//PM0-15 => DATA15-0
	PORT->PORTM_INEN = 0xFFFF;
	
	PORT->PORTM_SEL1 = 0xAAA;			//PM16 => OEN��PM17 => WEN��PM18 => NORFL_CSN��PM19 => SDRAM_CSN��PM20 => SRAM_CSN��PM21 => SDRAM_CKE
	
	SRAM_InitStruct.ClkDiv = SRAM_CLKDIV_8;		//��Ƶ40MHzʱ��ѡSRAM_CLKDIV_5����Ƶ120MHzʱһ��ѡSRAM_CLKDIV_8
	SRAM_InitStruct.DataWidth = SRAM_DATAWIDTH_16;
	SRAM_Init(&SRAM_InitStruct);
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
