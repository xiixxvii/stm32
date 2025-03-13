

/*----------------------------------------------------------------------------------*/

#include "framework.h"

static void fk_drv_btim0_Init(void);
static void fk_drv_btim0_Run(void);
static void fk_drv_btim0_Stop(void);

static void fk_drv_btim0_SetIntFunc(IntFunc0 func);				//�����жϵ���



fk_drv_btim0_t DrvBtim0={
	.Init 	= fk_drv_btim0_Init,
	.Run 		= fk_drv_btim0_Run,
	.Stop 	= fk_drv_btim0_Stop,
	.SetIntFunc = fk_drv_btim0_SetIntFunc,
};

static unsigned int gTimer0Tick;

//�˴�����ΪĬ���жϺ�������֪�����ߣ���û�����ô˺���
//�벻Ҫ���ж���ʹ�ô��ڣ���Ϊ�����ǻ����Ӳ���豸�����Ҵ����ٶȻ������жϡ�
static void IntError(void){gTimer0Tick++;printf("gTimer0Tick = %d \r\n",gTimer0Tick);}

static IntFunc0 thisDrvIntFunc = IntError; 								//�жϵ��ú���ָ��
/*----------------------------------------------------------------------------------*/
static void fk_drv_btim0_Init(void)
{
	/*����Ԥ��Ƶֵ*/
	unsigned char PerDiv = 48-1;		
	/*���ö�ʱ����ֵ*/
	unsigned int CounterVal = 1000-1;
	/*��ϲ���*/
	unsigned int periodVal =  PerDiv;
	periodVal  = periodVal << 24;
	periodVal  = periodVal  + CounterVal;
	/*Ӧ������*/
	TIMR_Init(BTIMR0, TIMR_MODE_TIMER, periodVal, 1);
}
/*----------------------------------------------------------------------------------*/
/*������ʱ��*/
static void fk_drv_btim0_Run(void)		{TIMR_Start(BTIMR0);}
/*ֹͣ��ʱ��*/
static void fk_drv_btim0_Stop(void)		{TIMR_Stop(BTIMR0);}

/*----------------------------------------------------------------------------------*/
//�����жϵ���
static void fk_drv_btim0_SetIntFunc(IntFunc0 func)
{
	thisDrvIntFunc = func;
}

/*----------------------------------------------------------------------------------*/


//static unsigned int fk_drv_btim0_GetTick(void)
//{
//	unsigned int iTick=0;
//	NVIC_DisableIRQ(BTIMR0_IRQn);
//	iTick = gTimer0Tick;
//	NVIC_EnableIRQ(BTIMR0_IRQn);
//	return iTick;
//}
/*----------------------------------------------------------------------------------*/

void BTIMR0_Handler(void)
{
	TIMR_INTClr(BTIMR0);
	thisDrvIntFunc();
}
/*----------------------------------------------------------------------------------*/

