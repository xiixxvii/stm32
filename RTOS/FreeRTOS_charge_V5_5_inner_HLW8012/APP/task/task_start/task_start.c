#include "task_inc.h"
#include "framework.h"
#include <stdlib.h>
#include "work.h"

#include <flashdb.h>

__task_body_start;


static task_body_t m_task_body={
	.name		= "start_task",
	.cn_name	= "��ʼ����",
	.stack_size	= 4096,
	.priority = 3,
	.handle = &handle,
	.create = create,
	
}; 





/*
(1)UART������������/��������
(2)PWM����		
(3)����������	
(4)OUTPIN����	
(5)ADC����

��Ϊ������


�����߼�
(1)������ʼ��3����֮�ڣ����жϳ�����ֻ�жϰε�
(2)��ʼ��������40sʱ��ĵȴ��ڣ�	
	��40sҲ��ͳ�Ƶ��ܵģ����ǲ��ж��Ƿ�ε�/δ�����õ�����
(3)�ε���жϷ�ʽ,����һ��ʱ��(2s)��û�еȵ��½��أ�����Ϊ���ε���
(4)�������жϱ�׼��1���ӳ����ܣ�����15W*60s 



*/




static void init_io_pin(void)
{
	

	gpio->set_mode( IO_SLED, PIN_MODE_OUTPUT );
	gpio->set_mode( IO_NetRst, PIN_MODE_OUTPUT );	
	
	gpio->set_mode( IO_ELC0, PIN_MODE_OUTPUT );
	gpio->set_mode( IO_ELC1, PIN_MODE_OUTPUT_OD );
	
	gpio->set_mode( IO_fuse0, PIN_MODE_INPUT );
	gpio->set_mode( IO_fuse1, PIN_MODE_INPUT );

//	gpio->set_mode( Hight0, PIN_MODE_INPUT );
//	gpio->set_mode( Hight1, PIN_MODE_INPUT );
	
	gpio->write( IO_ELC0, PIN_LOW );
	gpio->write( IO_ELC1, PIN_LOW );
	
}



//-------------------------------------------------------------------------

char* build_date = __DATE__;
char* build_time = __TIME__;


static void show_version_info(void)
{
	char vbuf[30];
	
	printf("\r\n");
	for(int i=0;i<LCD_XPixel;i++)	printf("-");
	printf("\r\n");
	
//	dot_ui->init();
//	dot_ui->show_ascii_str(0,0,"new app");
//	dot_ui->show();

	printf( "build:%s-%s\r\n",build_date,build_time );
	
	sprintf( vbuf,"%d_%d-%d_%d",
		Brd_Version,Brd_SubVersion,
		App_Version,App_SubVersion
	);
	
	printf( "version:%s\r\n",vbuf );
	
	printf("\r\n");
	for(int i=0;i<LCD_XPixel;i++)	printf("-");
	printf("\r\n");
	
	printf("SystemCoreClock = %d\r\n",SystemCoreClock);
}

//�ϵ�������ʼ��һ��RTC
static void init_rtc(void)
{
	rtc_dt.Year = 2024;
	rtc_dt.Month = 10;
	rtc_dt.Date = 10;
	rtc_dt.Hour = 10;
	rtc_dt.Minute = 10;
	rtc_dt.Second = 10;
	
	rtc->stop();
	rtc->init( &rtc_dt,0,0 );
	rtc->start();
	
}

static void task_entry(void *arg)
{
	//��ʼ������
	sysprt->init();
	uart0->init( 115200 );
	
	init_io_pin();
	
	
	//��ʾ�����Ϣ
	show_version_info();
	
	//host������������
	task_host->create(NULL,NULL);
	
	
	
//	eep_24c0x->init();
	vTaskDelay(100);
	
	wdat_guide_init();
	sysprt->alog("~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n" );
	
	init_rtc();
	
	work_data_load();							//���ع�������
	cmdline_man->boot_wait();
	
	sysprt->alog("run...\r\n");
	uart1->init( 115200 );
//	uart2->init( 4800 );
//	uart3->init( 4800 );


//	coder->init();
//	coder->set_pin( 0,Hight0 );
//	coder->config( 0,__Enable,500 );	//һ������ഥ�����ٴ�
//	coder->start( 0 );

//	coder->set_pin( 1,Hight1 );
//	coder->config( 1,__Enable,500 );	//һ������ഥ�����ٴ�
//	coder->start( 1 );

	

	hc595_opt->init( LED_BUS );
	
	
	sock_hlw_opt->init();
	ipool->init();
	
	//-------------------------------------------------------------------------
	//���濪�����ַ���
	
	//����listen����
	task_listen->create(NULL,NULL);
//	while(1);
	
	//����ctrl����
	task_ctrl->create( NULL,NULL );
	
	vTaskDelay(100);
	
	

	while(1)
	{
		if( 
			( wdat->net_mode == NetMode_Air724 ) || 
			( wdat->net_mode == NetMode_Air780 )
		)
		{
			work_internet();
		}
		
		vTaskDelay(5);
		
	}
}


__task_body_end;

//�����ļ���task_body���Ÿ�����ļ�����
__task_body_quote_to( task_start );









