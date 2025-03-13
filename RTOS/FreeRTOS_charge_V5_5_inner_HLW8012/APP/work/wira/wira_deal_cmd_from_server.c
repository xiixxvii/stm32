
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	
//static uint8_t send_buf[512];

#define MaxArgc_Normal  10		//��������������
#define MaxArgc_Time		30		//ʱ������������
#define PRT_FuncName( str ) { PRT_LOG("call %s:%s\r\n",__FUNCTION__,str); }
//-------------------------------------------------------------------------
static int deal_message_logon( uint8_t* dat )
{
	PRT_FuncName( dat );
	//���ջص�������˵����Ҫ�����������
	
	return 0;
}
//-------------------------------------------------------------------------
//�������������relogin
//�Թ�Ҫ����������յ�����Ҫ�������ӷ�������
static int deal_message_relogin( uint8_t* dat )
{
	PRT_FuncName( dat );
	//���ջص�
	
	int result = 0,i=0;
	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "relogin";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	//��Ҫ���»�ȡ��ʱʱ��
	guide_dat->time_slot->get_slot = 0;
	
	return ret_ReConnect;	//��������
}


//-------------------------------------------------------------------------
//�������������nologin
//�����ϣ��������������˵���ͻ��˱��������ߵ��ˡ���Ҫ����TCP�����ӷ�������
static int deal_message_nologin( uint8_t* dat )
{
	PRT_FuncName( dat );
	//���ջص�������˵����Ҫ�����������
	int result = 0,i=0;
	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "nologin";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	return ret_ReConnect;	//��������
}
//-------------------------------------------------------------------------
static int deal_message_over( uint8_t* dat )
{
	PRT_FuncName( dat );

	char ddh[MaxDDH_Len];
	pool_pt pool;
	
	int result = 0,i=0;
	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "over";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	//���ddh
	for( i=0;i<MaxDDH_Len;i++ ) ddh[i] = 0;
	for( i=0;i<strlen( argv[1] );i++ ) ddh[i] = argv[1][i];
	
	
	pool = ipool->get_by_ddh( ddh );
	if( pool != NULL )
	{
		PRT_LOG("\tdelate_one pool,ddh = %s \r\n",pool->event.ddh );
		ipool->delate_one	( pool );	//�ӽ�������ɾ��
		ipool->destroy		( pool ); //���״��ڴ����ͷ�
	}
	else
	{
		PRT_LOG("can't find ddh = %s \r\n",pool->event.ddh );
	}
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_log( uint8_t* dat )
{
	PRT_FuncName( dat );
	//���ջص�������˵����Ҫ�����������
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_pong( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_PARAM( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_STATU( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}

//-------------------------------------------------------------------------
//���ƽ�� ��ʱʱ��
static uint32_t check_sum_str(char* str)
{
	int i = 0;
	uint32_t sum = 0;
	for (i = 0; str[i] != '\0'; i++) {
			// ����ַ��Ƿ�Ϊ���֣�'0'��'9'�ķ�Χ��
			if (str[i] >= '0' && str[i] <= '9') {
					// ���ַ�ת��Ϊ���ֲ��ۼ�
					sum += str[i] - '0'; // '0'��ASCIIֵΪ48����ȥ�����ַ�ת��Ϊ��Ӧ������
			}
	}
	return sum;
}
//jfpg 10:30-12:00 12:00-14:00,18:00-19:00 14:00-16:00 16:00-18:00,23:00-24:00,00:01-01:00 75
extern  void time_slot_dump(int group,char* str);

static int deal_message_jfpg( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	int result = 0;

	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "jfpg";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	
	//�ж��Ƿ�Ϸ�
	uint32_t recv_check_sum = atol( argv[argc-1] );
	uint32_t my_check_sum = 0; 
	for( int i=0;i<(argc-1);i++ )
	{
		my_check_sum += check_sum_str( argv[i] );
	}
	PRT_LOG("my_check_sum = %d \r\n",my_check_sum);
	if( my_check_sum != recv_check_sum ) return -2;
	
	PRT_LOG("dump jfpg\r\n");
	
	//���ʱ�β��
	char* time_argv[ MaxArgc_Normal ];
	uint8_t time_argc = 0;
	
	guide_dat->time_slot->segments = 0;			//
	guide_dat->time_slot->groups = argc - 2;
	for( int group=1;group<(argc-1);group++ )
	{
		cmd_line = argv[group];
		time_argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, time_argv,",\t\n\r");
		PRT_LOG("Group[%d]:\r\n",group-1);
		for( int i=0;i<time_argc;i++ )	
		{
			time_slot_dump( group-1,time_argv[i] );	//group��0��ʼ���
			PRT_LOG( "\t time_argv[%d] = %s\r\n",i,time_argv[i] );
		}
		
	}
	
	
	guide_dat->time_slot->get_slot = 1;		//��ʱʱ�λ�ȡ�ɹ�
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_time( uint8_t* dat )
{
	PRT_FuncName( dat );

	int result = 0;

	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "time";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	//�ж��Ƿ�Ϸ�
	uint32_t recv_check_sum = atol( argv[argc-1] );
	uint32_t my_check_sum = 0; 
	for( int i=0;i<(argc-1);i++ )
	{
		my_check_sum += check_sum_str( argv[i] );
	}
	PRT_LOG("my_check_sum = %d \r\n",my_check_sum);
	if( my_check_sum != recv_check_sum ) return -2;
	
	//�������
	dump_rtc_by_str( argv[1] );
	
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_setparam( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_getstatu( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_update( uint8_t* dat )
{
	PRT_FuncName( dat );
	char ddh[MaxDDH_Len];
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;

	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{sysprt->alog( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "update";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}
	
	{//�˴�������һ��ά����¼������������ά����¼���ͳ����У�����OTAģʽ
		
		PRT_LOG("req ota\r\n" );
		for( i=0;i<MaxDDH_Len;i++ ) 	{ddh[i] = 0;	}
		sock_mq->put_event( ddh,_event_m_tenance,0x00,0x00,0,0,PNULL);
	}
	return 0;
}

//-------------------------------------------------------------------------
//�µ�����
/*
	�������̣��µ�ʱ�����ݵ�ǰʱ��+����ʱ��������õ�����ʱ�䡣
	���Ѵ�ʱ�ı����Ϣ�洢��flash�С�
	
*/
static int deal_message_on( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;
	
	uint8_t com = 0;			//�������Ƕȣ�����id
	uint8_t id = 0;				//Ӳ���Ƕȣ�����id
	uint32_t time = 0;		//ʱ��
	uint32_t len = 0;			//��ʱ������
	event_t event;
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{sysprt->alog( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "on";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}

	//������Ϣ
	com = atol( argv[1] );							//��ȡ����id
	if( com >= (MaxSock+1) ) return -1;	//��������������id��1��ʼ��
	id = com - 1;
	
	time = atol( argv[2] );							//��ȡʱ��
	if( time == 0 ) time = 10*60;				//��λ ����
	else if( time > (12*60) )						//����һ�·��������ܷ�̫��ʱ��
	{
		time = 10*60;
	}
	
	
	len = strlen( argv[3] );						//��ȡ�����ų���
	if( len>=30 ) len =30;
	//������
	for( i=0;i<MaxDDH_Len;i++ ) 	{event.ddh[i] = 0;	}
	for( i=0;i<len;i++ )					{	event.ddh[i] = argv[3][i];	}
	
	sysprt->alog( "DDH ->%s \r\n",event.ddh );
	//��һ����d����Ϣ��������,��÷�����һֱ��
	event.num = id;								//����id ��Ӳ���ϵĲ���id
	event.val = 0;								//��ʹ��
	event.dat = 1;								//������� �̶�1
	event.curr = 0;								//ƽ������ 0
	event_report_curr( &event );	//�ѵ������� ���ͳ�ȥ 
	//�˴����޸� 		guide_dat->rtd_num->sock[road]���������ַ���sock_ctrl����sock_ctrl����
	
	//�Ѷ�����Ϣ����sock_ctrl
	sock_mq->send_order( event.ddh,id,oct_start_charge,time,0 );
		

		
	return 0;
}

static int deal_message_off( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;
	
	uint8_t com = 0;			//�������Ƕȣ�����id
	uint8_t id = 0;				//Ӳ���Ƕȣ�����id
	uint32_t time = 0;		//ʱ��
	uint32_t len = 0;			//��ʱ������

	
	char ddh[MaxDDH_Len];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	char* token = "off";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}
	
	com = atol( argv[1] );							//��ȡ����id
	if( com >= (MaxSock+1) ) return -1;	//��������������id��1��ʼ��
	id = com - 1;
	
	len = strlen( argv[2] );						//��ȡ�����ų���
	if( len>=30 ) len =30;
	for( i=0;i<MaxDDH_Len;i++ ) 	{ddh[i] = 0;	}
	for( i=0;i<MaxDDH_Len;i++ ) 	{ddh[i] = argv[2][i];	}
	
	//�Ѷ�����Ϣ����sock_ctrl
	sock_mq->send_order( ddh,id,oct_cancel_charge,0,0 );
	
	return 0;
}

static int deal_message_reboot( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;
	
	uint8_t com = 0;			//�������Ƕȣ�����id
	uint8_t id = 0;				//Ӳ���Ƕȣ�����id
	uint32_t time = 0;		//ʱ��
	uint32_t len = 0;			//��ʱ������

	
	char ddh[MaxDDH_Len];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	char* token = "reboot";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}
	
	reset_system();
	
	return 0;
}
//=========================================================================

deal_srv_cmd_t wira_deal_cmd[_srv_cmd_max] = {
	deal_message_logon,							
	deal_message_relogin,
	deal_message_nologin,
	deal_message_over,
	deal_message_log,
	deal_message_pong,
	deal_message_PARAM,
	deal_message_STATU,
	deal_message_jfpg,
	deal_message_time,
	deal_message_setparam,
	deal_message_getstatu,
	deal_message_update,
	deal_message_on,
	deal_message_off,
	deal_message_reboot,
	
};

//���ʵ�ڸ㲻������Щ��������Լ�һ���ո񣬱������ʶ�𣬻��ߵ����Ⱥ�˳��
char* _srv_cmd_list[_srv_cmd_max]={
	"logon",
	"relogin",
	"nologin",
	"over",
	"log",
	"pong",
	"PARAM",
	"STATU",
	"jfpg",
	"time",
	"setparam",
	"getstatu",
	"update",
	"on",
	"off",
	"reboot",
	
};

//-------------------------------------------------------------------------
