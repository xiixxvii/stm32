/*----------------------------------------------------------------------------------*/

#include "framework.h"
/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Init(void);
static void fk_drv_spi0_Open(void);
static void fk_drv_spi0_Close(void);
static unsigned char fk_drv_spi0_Swap(unsigned char Dat);
static void fk_drv_spi0_Div(unsigned char div);
fk_drv_spi0_t DrvSpi0={
	.Init 	= fk_drv_spi0_Init,
	.Open 	= fk_drv_spi0_Open,
	.Close 	= fk_drv_spi0_Close,
	.Swap 	= fk_drv_spi0_Swap,
	.Div 		= fk_drv_spi0_Div,
};
/*����һ��SPI���ýṹ�壬����SPI0������*/
static 	SPI_InitStructure SPI_initStruct;

/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Init(void)
{
	/*��ʼ��SPI��������*/
	//PORT_Init(PORTA, PIN8,  PORTA_PIN8_SPI0_SSEL,  0);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_SPI0_SCLK, 0);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_SPI0_MOSI, 0);
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_SPI0_MISO,  1);
	//GPIO_Init(GPIOC, PIN1, 1, 1, 0, 0); //CS

	/*��Ƶ������SPIģ�鹤���ٶ�*/
	SPI_initStruct.clkDiv 				= SPI_CLKDIV_256;		//1.5MHz
	/*����SPIģ�鹤��ģʽ:SPIģʽ����TI_SSIģʽ*/
	SPI_initStruct.FrameFormat 		= SPI_FORMAT_SPI;		//ѡ��SPIģʽ
	/*������SPI֡��ʽ�µĲ���ʱ��:�����ز��������½��ز���
		��һ�������������أ��ڶ����������½���*/
	SPI_initStruct.SampleEdge 		= SPI_FIRST_EDGE;		//ѡ���һ������
	/*������SPI֡��ʽ�£�����ʱ�ĵ�ƽ:*/
	SPI_initStruct.IdleLevel 			= SPI_LOW_LEVEL;		//����ʱΪ�͵�ƽ
	/*�����ֳ���*/
	SPI_initStruct.WordSize 			= 8;
	/*�����Ƿ�ʹ��SPI����ģʽ*/
	SPI_initStruct.Master 				= 1;
	/*�����ж��Ƿ�ʹ��*/
	SPI_initStruct.RXThresholdIEn = 0;
	/*�����ж��Ƿ�ʹ��*/
	SPI_initStruct.TXThresholdIEn = 0;
	/*�Ƿ�����TX FIFO������λ�Ĵ����յ��ж�����*/
	SPI_initStruct.TXCompleteIEn  = 0;
	/*Ӧ����Щ����*/
	SPI_Init(SPI0, &SPI_initStruct);
}
/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Div(unsigned char div)
{
	SPI_initStruct.clkDiv 				= div;		//1.5MHz
	SPI_Init(SPI0, &SPI_initStruct);
}
/*----------------------------------------------------------------------------------*/
/*ʹ��SPIģ��*/
static void fk_drv_spi0_Open(void)												{SPI_Open(SPI0);}
/*----------------------------------------------------------------------------------*/
static void fk_drv_spi0_Close(void)												{SPI_Close(SPI0);}
/*----------------------------------------------------------------------------------*/
static unsigned char fk_drv_spi0_Swap(unsigned char Dat)	{return SPI_ReadWrite(SPI0,Dat);}
/*----------------------------------------------------------------------------------*/

