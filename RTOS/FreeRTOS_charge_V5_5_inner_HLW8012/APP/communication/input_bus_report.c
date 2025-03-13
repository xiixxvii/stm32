

#include "task_inc.h"
#include "framework.h"

#include "communication.h"

static int en_cycle_report = __Disable;				//������ʱ�㱨 ���ܿ���


//��ʱ�㱨 ���ܿ���
static void cycle_report_set(uint8_t enable)
{
	if( enable == __Disable )
	{
		en_cycle_report = __Disable;
	}
	else
	{
		en_cycle_report = __Enable;
	}
}

//��ʱ�㱨 ִ�к���
static uint8_t pin[8];
static 	time_t otick=0,ntick=0;
static void cycle_report(time_t tick)
{
}


/*----------------------------------------------------------------------------------*/
static uint8_t conf_event[64];							//�������õ��¼�ʱʲô
//�¼��㱨 ���ܿ���
static void event_report_set(uint8_t pin,uint8_t event)
{
	int i = 0;
	
	if( pin == 0XFF )
	{//����ȫ��
		for( i=0;i<64;i++ )
			conf_event[i] = event;
		return;
	}
	if( pin <64  )
	{//��������
		conf_event[i] = event;
	}

}


static uint8_t nevent_pin[8]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
static uint8_t oevent_pin[8]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
static uint8_t sbuf[8];

//�¼��㱨 ִ�к���
static void event_report(void)
{
}


static inb_report_t do_inb_report ={
	.cycle_report_set = cycle_report_set,
	.cycle_report = cycle_report,
	.event_report_set = event_report_set,
	.event_report = event_report,
};

inb_report_pt inb_report=&do_inb_report;



