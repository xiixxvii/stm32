#include "SWM320.h"

#include "NT35510.h"


#define NT35510_CMD_COUSOR_X	0x2A00
#define NT35510_CMD_COUSOR_Y	0x2B00
#define NT35510_CMD_WR_DATA		0x2C00
#define NT35510_CMD_RD_DATA		0x2E00


#define TFT_REG    *((volatile uint16_t *)(SRAMM_BASE + 0))		// TFT_RS �� EMC_A2

#define TFT_DATA   *((volatile uint16_t *)(SRAMM_BASE + 4))


/****************************************************************************************************************************************** 
* ��������:	TFTInit()
* ����˵��: TFTҺ������ʼ����TFTʹ��NT35510�������ֱ���800*480��ʹ������ԭ��4.3��������
*			��Ļģʽ�������������ҡ����ϵ���
* ��    ��: ��
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void TFTInit(void)
{
	uint32_t i;
	uint16_t id;
	SRAM_InitStructure SRAM_InitStruct;
	
	PORT->PORTP_SEL0 = 0xAAAAAAAA;		//PP0-23 => ADDR0-23
	PORT->PORTP_SEL1 = 0xAAAA;
	
	PORT->PORTM_SEL0 = 0xAAAAAAAA;		//PM0-15 => DATA15-0
	PORT->PORTM_INEN = 0xFFFF;
	
	PORT->PORTM_SEL1 = 0xAAA;			//PM16 => OEN��PM17 => WEN��PM18 => NORFL_CSN��PM19 => SDRAM_CSN��PM20 => SRAM_CSN��PM21 => SDRAM_CKE
	
	SRAM_InitStruct.ClkDiv = SRAM_CLKDIV_16;
	SRAM_InitStruct.DataWidth = SRAM_DATAWIDTH_16;
	SRAM_Init(&SRAM_InitStruct);
	
	GPIO_Init(GPIOC, PIN6, 1, 0, 0);	// ����
	GPIO_SetBit(GPIOC, PIN6);
	
	GPIO_Init(GPIOC, PIN7, 1, 0, 0);	// ��λ
	GPIO_ClrBit(GPIOC, PIN7);
	for(i = 0; i < 5000000; i++) __NOP();
	GPIO_SetBit(GPIOC, PIN7);
	for(i = 0; i < 1000000; i++) __NOP();
	
	LCD_I80_ReadReg(0xDA00);
	id = LCD_I80_ReadReg(0xDB00);
	id = (id << 8) | LCD_I80_ReadReg(0xDC00);
	if(id != 0x8000)
		return;
	
	LCD_I80_WriteReg(0xF000,0x55);
	LCD_I80_WriteReg(0xF001,0xAA);
	LCD_I80_WriteReg(0xF002,0x52);
	LCD_I80_WriteReg(0xF003,0x08);
	LCD_I80_WriteReg(0xF004,0x01);
			
	//AVDD Set AVDD 5.2V
	LCD_I80_WriteReg(0xB000,0x0D);
	LCD_I80_WriteReg(0xB001,0x0D);
	LCD_I80_WriteReg(0xB002,0x0D);
	//AVDD ratio
	LCD_I80_WriteReg(0xB600,0x34);
	LCD_I80_WriteReg(0xB601,0x34);
	LCD_I80_WriteReg(0xB602,0x34);
	//AVEE -5.2V
	LCD_I80_WriteReg(0xB100,0x0D);
	LCD_I80_WriteReg(0xB101,0x0D);
	LCD_I80_WriteReg(0xB102,0x0D);
	//AVEE ratio
	LCD_I80_WriteReg(0xB700,0x34);
	LCD_I80_WriteReg(0xB701,0x34);
	LCD_I80_WriteReg(0xB702,0x34);
	//VCL -2.5V
	LCD_I80_WriteReg(0xB200,0x00);
	LCD_I80_WriteReg(0xB201,0x00);
	LCD_I80_WriteReg(0xB202,0x00);
	//VCL ratio
	LCD_I80_WriteReg(0xB800,0x24);
	LCD_I80_WriteReg(0xB801,0x24);
	LCD_I80_WriteReg(0xB802,0x24);
	//VGH 15V (Free pump)
	LCD_I80_WriteReg(0xBF00,0x01);
	LCD_I80_WriteReg(0xB300,0x0F);
	LCD_I80_WriteReg(0xB301,0x0F);
	LCD_I80_WriteReg(0xB302,0x0F);
	//VGH ratio
	LCD_I80_WriteReg(0xB900,0x34);
	LCD_I80_WriteReg(0xB901,0x34);
	LCD_I80_WriteReg(0xB902,0x34);
	//VGL_REG -10V
	LCD_I80_WriteReg(0xB500,0x08);
	LCD_I80_WriteReg(0xB501,0x08);
	LCD_I80_WriteReg(0xB502,0x08);
	LCD_I80_WriteReg(0xC200,0x03);
	//VGLX ratio
	LCD_I80_WriteReg(0xBA00,0x24);
	LCD_I80_WriteReg(0xBA01,0x24);
	LCD_I80_WriteReg(0xBA02,0x24);
	//VGMP/VGSP 4.5V/0V
	LCD_I80_WriteReg(0xBC00,0x00);
	LCD_I80_WriteReg(0xBC01,0x78);
	LCD_I80_WriteReg(0xBC02,0x00);
	//VGMN/VGSN -4.5V/0V
	LCD_I80_WriteReg(0xBD00,0x00);
	LCD_I80_WriteReg(0xBD01,0x78);
	LCD_I80_WriteReg(0xBD02,0x00);
	//VCOM
	LCD_I80_WriteReg(0xBE00,0x00);
	LCD_I80_WriteReg(0xBE01,0x64);
	//Gamma Setting
	LCD_I80_WriteReg(0xD100,0x00);
	LCD_I80_WriteReg(0xD101,0x33);
	LCD_I80_WriteReg(0xD102,0x00);
	LCD_I80_WriteReg(0xD103,0x34);
	LCD_I80_WriteReg(0xD104,0x00);
	LCD_I80_WriteReg(0xD105,0x3A);
	LCD_I80_WriteReg(0xD106,0x00);
	LCD_I80_WriteReg(0xD107,0x4A);
	LCD_I80_WriteReg(0xD108,0x00);
	LCD_I80_WriteReg(0xD109,0x5C);
	LCD_I80_WriteReg(0xD10A,0x00);
	LCD_I80_WriteReg(0xD10B,0x81);
	LCD_I80_WriteReg(0xD10C,0x00);
	LCD_I80_WriteReg(0xD10D,0xA6);
	LCD_I80_WriteReg(0xD10E,0x00);
	LCD_I80_WriteReg(0xD10F,0xE5);
	LCD_I80_WriteReg(0xD110,0x01);
	LCD_I80_WriteReg(0xD111,0x13);
	LCD_I80_WriteReg(0xD112,0x01);
	LCD_I80_WriteReg(0xD113,0x54);
	LCD_I80_WriteReg(0xD114,0x01);
	LCD_I80_WriteReg(0xD115,0x82);
	LCD_I80_WriteReg(0xD116,0x01);
	LCD_I80_WriteReg(0xD117,0xCA);
	LCD_I80_WriteReg(0xD118,0x02);
	LCD_I80_WriteReg(0xD119,0x00);
	LCD_I80_WriteReg(0xD11A,0x02);
	LCD_I80_WriteReg(0xD11B,0x01);
	LCD_I80_WriteReg(0xD11C,0x02);
	LCD_I80_WriteReg(0xD11D,0x34);
	LCD_I80_WriteReg(0xD11E,0x02);
	LCD_I80_WriteReg(0xD11F,0x67);
	LCD_I80_WriteReg(0xD120,0x02);
	LCD_I80_WriteReg(0xD121,0x84);
	LCD_I80_WriteReg(0xD122,0x02);
	LCD_I80_WriteReg(0xD123,0xA4);
	LCD_I80_WriteReg(0xD124,0x02);
	LCD_I80_WriteReg(0xD125,0xB7);
	LCD_I80_WriteReg(0xD126,0x02);
	LCD_I80_WriteReg(0xD127,0xCF);
	LCD_I80_WriteReg(0xD128,0x02);
	LCD_I80_WriteReg(0xD129,0xDE);
	LCD_I80_WriteReg(0xD12A,0x02);
	LCD_I80_WriteReg(0xD12B,0xF2);
	LCD_I80_WriteReg(0xD12C,0x02);
	LCD_I80_WriteReg(0xD12D,0xFE);
	LCD_I80_WriteReg(0xD12E,0x03);
	LCD_I80_WriteReg(0xD12F,0x10);
	LCD_I80_WriteReg(0xD130,0x03);
	LCD_I80_WriteReg(0xD131,0x33);
	LCD_I80_WriteReg(0xD132,0x03);
	LCD_I80_WriteReg(0xD133,0x6D);
	LCD_I80_WriteReg(0xD200,0x00);
	LCD_I80_WriteReg(0xD201,0x33);
	LCD_I80_WriteReg(0xD202,0x00);
	LCD_I80_WriteReg(0xD203,0x34);
	LCD_I80_WriteReg(0xD204,0x00);
	LCD_I80_WriteReg(0xD205,0x3A);
	LCD_I80_WriteReg(0xD206,0x00);
	LCD_I80_WriteReg(0xD207,0x4A);
	LCD_I80_WriteReg(0xD208,0x00);
	LCD_I80_WriteReg(0xD209,0x5C);
	LCD_I80_WriteReg(0xD20A,0x00);

	LCD_I80_WriteReg(0xD20B,0x81);
	LCD_I80_WriteReg(0xD20C,0x00);
	LCD_I80_WriteReg(0xD20D,0xA6);
	LCD_I80_WriteReg(0xD20E,0x00);
	LCD_I80_WriteReg(0xD20F,0xE5);
	LCD_I80_WriteReg(0xD210,0x01);
	LCD_I80_WriteReg(0xD211,0x13);
	LCD_I80_WriteReg(0xD212,0x01);
	LCD_I80_WriteReg(0xD213,0x54);
	LCD_I80_WriteReg(0xD214,0x01);
	LCD_I80_WriteReg(0xD215,0x82);
	LCD_I80_WriteReg(0xD216,0x01);
	LCD_I80_WriteReg(0xD217,0xCA);
	LCD_I80_WriteReg(0xD218,0x02);
	LCD_I80_WriteReg(0xD219,0x00);
	LCD_I80_WriteReg(0xD21A,0x02);
	LCD_I80_WriteReg(0xD21B,0x01);
	LCD_I80_WriteReg(0xD21C,0x02);
	LCD_I80_WriteReg(0xD21D,0x34);
	LCD_I80_WriteReg(0xD21E,0x02);
	LCD_I80_WriteReg(0xD21F,0x67);
	LCD_I80_WriteReg(0xD220,0x02);
	LCD_I80_WriteReg(0xD221,0x84);
	LCD_I80_WriteReg(0xD222,0x02);
	LCD_I80_WriteReg(0xD223,0xA4);
	LCD_I80_WriteReg(0xD224,0x02);
	LCD_I80_WriteReg(0xD225,0xB7);
	LCD_I80_WriteReg(0xD226,0x02);
	LCD_I80_WriteReg(0xD227,0xCF);
	LCD_I80_WriteReg(0xD228,0x02);
	LCD_I80_WriteReg(0xD229,0xDE);
	LCD_I80_WriteReg(0xD22A,0x02);
	LCD_I80_WriteReg(0xD22B,0xF2);
	LCD_I80_WriteReg(0xD22C,0x02);
	LCD_I80_WriteReg(0xD22D,0xFE);
	LCD_I80_WriteReg(0xD22E,0x03);
	LCD_I80_WriteReg(0xD22F,0x10);
	LCD_I80_WriteReg(0xD230,0x03);
	LCD_I80_WriteReg(0xD231,0x33);
	LCD_I80_WriteReg(0xD232,0x03);
	LCD_I80_WriteReg(0xD233,0x6D);
	LCD_I80_WriteReg(0xD300,0x00);
	LCD_I80_WriteReg(0xD301,0x33);
	LCD_I80_WriteReg(0xD302,0x00);
	LCD_I80_WriteReg(0xD303,0x34);
	LCD_I80_WriteReg(0xD304,0x00);
	LCD_I80_WriteReg(0xD305,0x3A);
	LCD_I80_WriteReg(0xD306,0x00);
	LCD_I80_WriteReg(0xD307,0x4A);
	LCD_I80_WriteReg(0xD308,0x00);
	LCD_I80_WriteReg(0xD309,0x5C);
	LCD_I80_WriteReg(0xD30A,0x00);

	LCD_I80_WriteReg(0xD30B,0x81);
	LCD_I80_WriteReg(0xD30C,0x00);
	LCD_I80_WriteReg(0xD30D,0xA6);
	LCD_I80_WriteReg(0xD30E,0x00);
	LCD_I80_WriteReg(0xD30F,0xE5);
	LCD_I80_WriteReg(0xD310,0x01);
	LCD_I80_WriteReg(0xD311,0x13);
	LCD_I80_WriteReg(0xD312,0x01);
	LCD_I80_WriteReg(0xD313,0x54);
	LCD_I80_WriteReg(0xD314,0x01);
	LCD_I80_WriteReg(0xD315,0x82);
	LCD_I80_WriteReg(0xD316,0x01);
	LCD_I80_WriteReg(0xD317,0xCA);
	LCD_I80_WriteReg(0xD318,0x02);
	LCD_I80_WriteReg(0xD319,0x00);
	LCD_I80_WriteReg(0xD31A,0x02);
	LCD_I80_WriteReg(0xD31B,0x01);
	LCD_I80_WriteReg(0xD31C,0x02);
	LCD_I80_WriteReg(0xD31D,0x34);
	LCD_I80_WriteReg(0xD31E,0x02);
	LCD_I80_WriteReg(0xD31F,0x67);
	LCD_I80_WriteReg(0xD320,0x02);
	LCD_I80_WriteReg(0xD321,0x84);
	LCD_I80_WriteReg(0xD322,0x02);
	LCD_I80_WriteReg(0xD323,0xA4);
	LCD_I80_WriteReg(0xD324,0x02);
	LCD_I80_WriteReg(0xD325,0xB7);
	LCD_I80_WriteReg(0xD326,0x02);
	LCD_I80_WriteReg(0xD327,0xCF);
	LCD_I80_WriteReg(0xD328,0x02);
	LCD_I80_WriteReg(0xD329,0xDE);
	LCD_I80_WriteReg(0xD32A,0x02);
	LCD_I80_WriteReg(0xD32B,0xF2);
	LCD_I80_WriteReg(0xD32C,0x02);
	LCD_I80_WriteReg(0xD32D,0xFE);
	LCD_I80_WriteReg(0xD32E,0x03);
	LCD_I80_WriteReg(0xD32F,0x10);
	LCD_I80_WriteReg(0xD330,0x03);
	LCD_I80_WriteReg(0xD331,0x33);
	LCD_I80_WriteReg(0xD332,0x03);
	LCD_I80_WriteReg(0xD333,0x6D);
	LCD_I80_WriteReg(0xD400,0x00);
	LCD_I80_WriteReg(0xD401,0x33);
	LCD_I80_WriteReg(0xD402,0x00);
	LCD_I80_WriteReg(0xD403,0x34);
	LCD_I80_WriteReg(0xD404,0x00);
	LCD_I80_WriteReg(0xD405,0x3A);
	LCD_I80_WriteReg(0xD406,0x00);
	LCD_I80_WriteReg(0xD407,0x4A);
	LCD_I80_WriteReg(0xD408,0x00);
	LCD_I80_WriteReg(0xD409,0x5C);
	LCD_I80_WriteReg(0xD40A,0x00);
	LCD_I80_WriteReg(0xD40B,0x81);

	LCD_I80_WriteReg(0xD40C,0x00);
	LCD_I80_WriteReg(0xD40D,0xA6);
	LCD_I80_WriteReg(0xD40E,0x00);
	LCD_I80_WriteReg(0xD40F,0xE5);
	LCD_I80_WriteReg(0xD410,0x01);
	LCD_I80_WriteReg(0xD411,0x13);
	LCD_I80_WriteReg(0xD412,0x01);
	LCD_I80_WriteReg(0xD413,0x54);
	LCD_I80_WriteReg(0xD414,0x01);
	LCD_I80_WriteReg(0xD415,0x82);
	LCD_I80_WriteReg(0xD416,0x01);
	LCD_I80_WriteReg(0xD417,0xCA);
	LCD_I80_WriteReg(0xD418,0x02);
	LCD_I80_WriteReg(0xD419,0x00);
	LCD_I80_WriteReg(0xD41A,0x02);
	LCD_I80_WriteReg(0xD41B,0x01);
	LCD_I80_WriteReg(0xD41C,0x02);
	LCD_I80_WriteReg(0xD41D,0x34);
	LCD_I80_WriteReg(0xD41E,0x02);
	LCD_I80_WriteReg(0xD41F,0x67);
	LCD_I80_WriteReg(0xD420,0x02);
	LCD_I80_WriteReg(0xD421,0x84);
	LCD_I80_WriteReg(0xD422,0x02);
	LCD_I80_WriteReg(0xD423,0xA4);
	LCD_I80_WriteReg(0xD424,0x02);
	LCD_I80_WriteReg(0xD425,0xB7);
	LCD_I80_WriteReg(0xD426,0x02);
	LCD_I80_WriteReg(0xD427,0xCF);
	LCD_I80_WriteReg(0xD428,0x02);
	LCD_I80_WriteReg(0xD429,0xDE);
	LCD_I80_WriteReg(0xD42A,0x02);
	LCD_I80_WriteReg(0xD42B,0xF2);
	LCD_I80_WriteReg(0xD42C,0x02);
	LCD_I80_WriteReg(0xD42D,0xFE);
	LCD_I80_WriteReg(0xD42E,0x03);
	LCD_I80_WriteReg(0xD42F,0x10);
	LCD_I80_WriteReg(0xD430,0x03);
	LCD_I80_WriteReg(0xD431,0x33);
	LCD_I80_WriteReg(0xD432,0x03);
	LCD_I80_WriteReg(0xD433,0x6D);
	LCD_I80_WriteReg(0xD500,0x00);
	LCD_I80_WriteReg(0xD501,0x33);
	LCD_I80_WriteReg(0xD502,0x00);
	LCD_I80_WriteReg(0xD503,0x34);
	LCD_I80_WriteReg(0xD504,0x00);
	LCD_I80_WriteReg(0xD505,0x3A);
	LCD_I80_WriteReg(0xD506,0x00);
	LCD_I80_WriteReg(0xD507,0x4A);
	LCD_I80_WriteReg(0xD508,0x00);
	LCD_I80_WriteReg(0xD509,0x5C);
	LCD_I80_WriteReg(0xD50A,0x00);
	LCD_I80_WriteReg(0xD50B,0x81);

	LCD_I80_WriteReg(0xD50C,0x00);
	LCD_I80_WriteReg(0xD50D,0xA6);
	LCD_I80_WriteReg(0xD50E,0x00);
	LCD_I80_WriteReg(0xD50F,0xE5);
	LCD_I80_WriteReg(0xD510,0x01);
	LCD_I80_WriteReg(0xD511,0x13);
	LCD_I80_WriteReg(0xD512,0x01);
	LCD_I80_WriteReg(0xD513,0x54);
	LCD_I80_WriteReg(0xD514,0x01);
	LCD_I80_WriteReg(0xD515,0x82);
	LCD_I80_WriteReg(0xD516,0x01);
	LCD_I80_WriteReg(0xD517,0xCA);
	LCD_I80_WriteReg(0xD518,0x02);
	LCD_I80_WriteReg(0xD519,0x00);
	LCD_I80_WriteReg(0xD51A,0x02);
	LCD_I80_WriteReg(0xD51B,0x01);
	LCD_I80_WriteReg(0xD51C,0x02);
	LCD_I80_WriteReg(0xD51D,0x34);
	LCD_I80_WriteReg(0xD51E,0x02);
	LCD_I80_WriteReg(0xD51F,0x67);
	LCD_I80_WriteReg(0xD520,0x02);
	LCD_I80_WriteReg(0xD521,0x84);
	LCD_I80_WriteReg(0xD522,0x02);
	LCD_I80_WriteReg(0xD523,0xA4);
	LCD_I80_WriteReg(0xD524,0x02);
	LCD_I80_WriteReg(0xD525,0xB7);
	LCD_I80_WriteReg(0xD526,0x02);
	LCD_I80_WriteReg(0xD527,0xCF);
	LCD_I80_WriteReg(0xD528,0x02);
	LCD_I80_WriteReg(0xD529,0xDE);
	LCD_I80_WriteReg(0xD52A,0x02);
	LCD_I80_WriteReg(0xD52B,0xF2);
	LCD_I80_WriteReg(0xD52C,0x02);
	LCD_I80_WriteReg(0xD52D,0xFE);
	LCD_I80_WriteReg(0xD52E,0x03);
	LCD_I80_WriteReg(0xD52F,0x10);
	LCD_I80_WriteReg(0xD530,0x03);
	LCD_I80_WriteReg(0xD531,0x33);
	LCD_I80_WriteReg(0xD532,0x03);
	LCD_I80_WriteReg(0xD533,0x6D);
	LCD_I80_WriteReg(0xD600,0x00);
	LCD_I80_WriteReg(0xD601,0x33);
	LCD_I80_WriteReg(0xD602,0x00);
	LCD_I80_WriteReg(0xD603,0x34);
	LCD_I80_WriteReg(0xD604,0x00);
	LCD_I80_WriteReg(0xD605,0x3A);
	LCD_I80_WriteReg(0xD606,0x00);
	LCD_I80_WriteReg(0xD607,0x4A);
	LCD_I80_WriteReg(0xD608,0x00);
	LCD_I80_WriteReg(0xD609,0x5C);
	LCD_I80_WriteReg(0xD60A,0x00);
	LCD_I80_WriteReg(0xD60B,0x81);

	LCD_I80_WriteReg(0xD60C,0x00);
	LCD_I80_WriteReg(0xD60D,0xA6);
	LCD_I80_WriteReg(0xD60E,0x00);
	LCD_I80_WriteReg(0xD60F,0xE5);
	LCD_I80_WriteReg(0xD610,0x01);
	LCD_I80_WriteReg(0xD611,0x13);
	LCD_I80_WriteReg(0xD612,0x01);
	LCD_I80_WriteReg(0xD613,0x54);
	LCD_I80_WriteReg(0xD614,0x01);
	LCD_I80_WriteReg(0xD615,0x82);
	LCD_I80_WriteReg(0xD616,0x01);
	LCD_I80_WriteReg(0xD617,0xCA);
	LCD_I80_WriteReg(0xD618,0x02);
	LCD_I80_WriteReg(0xD619,0x00);
	LCD_I80_WriteReg(0xD61A,0x02);
	LCD_I80_WriteReg(0xD61B,0x01);
	LCD_I80_WriteReg(0xD61C,0x02);
	LCD_I80_WriteReg(0xD61D,0x34);
	LCD_I80_WriteReg(0xD61E,0x02);
	LCD_I80_WriteReg(0xD61F,0x67);
	LCD_I80_WriteReg(0xD620,0x02);
	LCD_I80_WriteReg(0xD621,0x84);
	LCD_I80_WriteReg(0xD622,0x02);
	LCD_I80_WriteReg(0xD623,0xA4);
	LCD_I80_WriteReg(0xD624,0x02);
	LCD_I80_WriteReg(0xD625,0xB7);
	LCD_I80_WriteReg(0xD626,0x02);
	LCD_I80_WriteReg(0xD627,0xCF);
	LCD_I80_WriteReg(0xD628,0x02);
	LCD_I80_WriteReg(0xD629,0xDE);
	LCD_I80_WriteReg(0xD62A,0x02);
	LCD_I80_WriteReg(0xD62B,0xF2);
	LCD_I80_WriteReg(0xD62C,0x02);
	LCD_I80_WriteReg(0xD62D,0xFE);
	LCD_I80_WriteReg(0xD62E,0x03);
	LCD_I80_WriteReg(0xD62F,0x10);
	LCD_I80_WriteReg(0xD630,0x03);
	LCD_I80_WriteReg(0xD631,0x33);
	LCD_I80_WriteReg(0xD632,0x03);
	LCD_I80_WriteReg(0xD633,0x6D);
	//LV2 Page 0 enable
	LCD_I80_WriteReg(0xF000,0x55);
	LCD_I80_WriteReg(0xF001,0xAA);
	LCD_I80_WriteReg(0xF002,0x52);
	LCD_I80_WriteReg(0xF003,0x08);
	LCD_I80_WriteReg(0xF004,0x00);
	//Display control
	LCD_I80_WriteReg(0xB100, 0xCC);
	LCD_I80_WriteReg(0xB101, 0x00);
	//Source hold time
	LCD_I80_WriteReg(0xB600,0x05);
	//Gate EQ control
	LCD_I80_WriteReg(0xB700,0x70);
	LCD_I80_WriteReg(0xB701,0x70);
	//Source EQ control (Mode 2)
	LCD_I80_WriteReg(0xB800,0x01);
	LCD_I80_WriteReg(0xB801,0x03);
	LCD_I80_WriteReg(0xB802,0x03);
	LCD_I80_WriteReg(0xB803,0x03);
	//Inversion mode (2-dot)
	LCD_I80_WriteReg(0xBC00,0x02);
	LCD_I80_WriteReg(0xBC01,0x00);
	LCD_I80_WriteReg(0xBC02,0x00);
	//Timing control 4H w/ 4-delay
	LCD_I80_WriteReg(0xC900,0xD0);
	LCD_I80_WriteReg(0xC901,0x02);
	LCD_I80_WriteReg(0xC902,0x50);
	LCD_I80_WriteReg(0xC903,0x50);
	LCD_I80_WriteReg(0xC904,0x50);
	LCD_I80_WriteReg(0x3500,0x00);
	LCD_I80_WriteReg(0x3A00,0x55);  //16-bit/pixel
	
	LCD_I80_WriteReg(0x1100,0x00);
	for(i = 0; i < 10000; i++) __NOP();
	LCD_I80_WriteReg(0x2900,0x00);	//LCD on
	
// 	LCD_I80_Init();
// #define LCD_I80_WriteReg(lcd, reg, data)  LCD_I80_GPIOWriteRegAndData(reg, data)
}


void LCD_I80_WriteReg(uint16_t reg, uint16_t data)
{
	TFT_REG = reg;
	
	TFT_DATA = data;
}


uint16_t LCD_I80_ReadReg(uint16_t reg)
{	
	TFT_REG = reg;
	
	return TFT_DATA;
}


void LCD_I80_SelectReg(uint16_t reg)
{
	TFT_REG = reg;
}


void LCD_I80_WriteData(uint16_t rgb)
{
	TFT_DATA = rgb;
}


uint16_t LCD_I80_ReadData(void)
{
	return TFT_DATA;
}


/****************************************************************************************************************************************** 
* ��������: TFTSetCursor()
* ����˵��: 
* ��    ��: 
* ��    ��: 
* ע������: 
******************************************************************************************************************************************/
void TFTSetCursor(uint16_t x, uint16_t y)
{
	LCD_I80_WriteReg(NT35510_CMD_COUSOR_X,   x>>8);
	LCD_I80_WriteReg(NT35510_CMD_COUSOR_X+1, x&0xFF);
	LCD_I80_WriteReg(NT35510_CMD_COUSOR_Y,   y>>8);
	LCD_I80_WriteReg(NT35510_CMD_COUSOR_Y+1, y&0xFF);
}


/****************************************************************************************************************************************** 
* ��������: TFTDrawPoint()
* ����˵��: 
* ��    ��: 
* ��    ��: 
* ע������: 
******************************************************************************************************************************************/
void TFTDrawPoint(uint16_t x, uint16_t y, uint16_t rgb)
{
	TFTSetCursor(x, y);
	
	LCD_I80_SelectReg(NT35510_CMD_WR_DATA);
	LCD_I80_WriteData(rgb);
}


/****************************************************************************************************************************************** 
* ��������: TFTReadPoint()
* ����˵��: 
* ��    ��: 
* ��    ��: 
* ע������: 
******************************************************************************************************************************************/
uint16_t TFTReadPoint(uint16_t x, uint16_t y)
{	
	uint16_t r8g8, b8__;
	
	TFTSetCursor(x, y);
	
	LCD_I80_SelectReg(NT35510_CMD_RD_DATA);
	LCD_I80_ReadData();	 // dummy read
	
	r8g8 = LCD_I80_ReadData();
	b8__ = LCD_I80_ReadData();
	
	return (r8g8 & 0xF800) | ((r8g8 & 0x00FC) << 3) | (b8__ >> 11);
}


/****************************************************************************************************************************************** 
* ��������: TFTClear()
* ����˵��: 
* ��    ��: 
* ��    ��: 
* ע������: 
******************************************************************************************************************************************/
void TFTClear(uint16_t rgb)
{
	uint32_t i, j;
	
	for(i = 0; i < 800; i++)
	{
		TFTSetCursor(0, i);
		
		LCD_I80_SelectReg(NT35510_CMD_WR_DATA);
		for(j = 0; j < 480; j++)
		{
			LCD_I80_WriteData(rgb);
		}
	}
}
