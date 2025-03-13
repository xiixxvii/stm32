
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	


static uint8_t send_buf[512];


/*-----------------------------------------------------------*/

void wira_event_report( void )
{
	pool_pt pool;
	
	event_t tevent;
//	uint8_t id = 0;
	
//	int ret = 0;
	int dump_len = 0;
	
//	int i = 0;
	if( sock_mq->recv_event( &tevent ) < 0  ) return;
	
	sysprt->aerr( "wira_event_report :");
	sysprt->aerr( "\tddh :%s",tevent.ddh );
	sysprt->aerr( "\ttype :%d",tevent.type );
	sysprt->aerr( "\tnum :%d",tevent.num );
	sysprt->aerr( "\tval :%d",tevent.val );
	sysprt->aerr( "\tdat :%d",tevent.dat );
	sysprt->aerr( "\tcurr :%f\r\n",tevent.curr );

	switch( tevent.type )
	{
		
		case _event_curr://�����ϱ�
			goto _do_event_curr;
			
		case _event_sock://�����¼�
			goto _do_event_sock;

		case _event_net://�����¼�
			goto _do_event_net;
		
		case _event_recore://������¼
			
			goto _do_event_recore;
		
		case _event_m_tenance://ά����־
			goto _do__event_m_tenance;
		default:
			break;
	}
	return;
	
_do_event_curr:
	//�������ý�����ֱ�ӷ���������
	event_report_curr( &tevent );		
	return;
	
_do_event_sock:
	pool = ipool->create( 50,30*1000,&tevent );
	ipool->register_one( pool );
	return;

_do_event_net:
	pool = ipool->create( 50,20*1000,&tevent );
	ipool->register_one( pool );
	return;
	
_do_event_recore:
	//������¼���ý�����ֱ�ӷ���������
	event_report_record( &tevent );		
	return;
	
_do__event_m_tenance:
	//ά����¼���ý�����ֱ�ӷ���������
	event_report_m_tenance( &tevent );	
	return;
}

/*-----------------------------------------------------------*/

void show_send_data( char* send_buf )
{
	sysprt->alog( "--------------->\r\n" );
	sysprt->alog( "send:%s",send_buf );
	sysprt->alog( "len :%d\r\n",strlen( (char*) send_buf) );
}


//���͵������ĸ�������
//ddh	->������
//num ->����id
//val ->�������
//dat ->û��ʹ��
//curr ->����ֵ
void event_report_curr( event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	char srtbuf[16];

	double kwh = 0;
	
	PRT_LOG("%s\r\n",__func__);
	uint8_t com = 0;		//�������Ƕ�, ������
	uint8_t id = 0;			//Ӳ���Ƕȣ�������
	uint32_t pwr = 0;
	uint32_t stime = 0,ntime = 0;	
	
	
	com = event->num; 	//���������ʱ����ǰ������(��������1��ʼ��)
	id = com - 1;				//��ǰ����ID 
	
	memset( send_buf,0,sizeof(send_buf) );
	
	//��䷢�� ������ �ϱ������������
	
	//������
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"LOG all " );
	dump_len += ret;
	
	//������
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),event->ddh );
	dump_len += ret;
	
	//���
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",event->dat );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;

	//��ǰ���ں�
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",com );					
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;

	//��һ��������״̬
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",get_sock_sta( 1 - (id&0x01) ) );		//��ȡ��һ��������״̬
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//�������ĵ���
	memset( srtbuf,0,sizeof( srtbuf ) );
	kwh = guide_dat->total_e->pul[id] * wdat->k[id]/3600/1000;
	sprintf( srtbuf," %.5f",kwh );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//ƽ������
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %f",event->curr );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//����
	pwr = event->curr*220;
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",pwr );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//��ѹ
	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 220" );
	dump_len += ret;
	
	//���ʱ��
	stime = datetime_to_minutes( guide_dat->order_info->sock[id].start );
	ntime = datetime_to_minutes( rtc_dt );

	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf," %d",(ntime - stime) );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	//�س�
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"\r\n" );
	dump_len += ret;

	air_man->send_str( air724, (char*)send_buf );	
	
	show_send_data( (char*)send_buf );
		
}
/*-----------------------------------------------------------*/
//����ά�����ĸ�������

void event_report_m_tenance( event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	
	memset( send_buf,0,sizeof(send_buf) );
	
	
	switch( event->num )
	{
		case 0x00://�������app״̬

			ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"UPDATE ok in 5\r\n" );
			dump_len += ret;
			
			air_man->send_str( air724, (char*)send_buf );	
			show_send_data( (char*)send_buf );
		
			//����������
			task_delay(10);
			sysprt->aerr("event_report_m_tenance:req_goto_update\r\n");
			wdat->program = _app_update;
			req_goto_update();
			break;
		case 0x01://����app�ɹ�����һ������ʱ�����߷�����
			ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"UPDATE ok ss 5\r\n" );
			dump_len += ret;
		
			air_man->send_str( air724, (char*)send_buf );	
			show_send_data( (char*)send_buf );
		
			break;
		default:
			break;
	}

	
}

/*-----------------------------------------------------------*/
//���Ͳ����¼����ĸ�������
void event_report_sock( event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	int group = 0;
	int max = 0;
	char srtbuf[32];
	uint32_t total_time_sec = 0;
	uint32_t group_time_sec = 0;
	float group_kwh = 0;
	
	memset( send_buf,0,sizeof(send_buf) );
	
	//��䷢�� ������ �ϱ������������
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"LOG over " );
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),event->ddh );
	dump_len += ret;

	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," " );
	dump_len += ret;

	
	total_time_sec = 0;
	//���ÿ����ĳ�����,ʱ��
	max = guide_dat->time_slot->groups;
	if( max> MaxPeriodE ) max = MaxPeriodE;
	for( group=0;group<max;group++ )
	{
		memset( srtbuf,0,sizeof( srtbuf ) );
		group_kwh = event->period_e[group].kwh;						//���õ� ��λ ǧ��ʱ
		group_time_sec = event->period_e[group].min*60;		//����ʱ ��λ s
		total_time_sec += group_time_sec; 								//����ʱ ��λ s
		
		sprintf( srtbuf,"%.5f,%d ",group_kwh,group_time_sec );
		ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
		dump_len += ret;
	}
	
	memset( srtbuf,0,sizeof( srtbuf ) );
	sprintf( srtbuf,"%d",total_time_sec );
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),srtbuf );
	dump_len += ret;
	
	
	//��������ԭ��
	switch( event->val )
	{
		case oeb_time:						ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00" );break;
		case oeb_curr_too_small:	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 01" );break;
		case oeb_curr_too_big:		ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 02" );break;
		case oeb_no_ins:					ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 03" );break;
		case oeb_fuse_broken:			ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 04" );break;
		case oeb_manual_stop:			ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 05" );break;
		default: 
															ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00" );break;
	}
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"\r\n" );
	dump_len += ret;
	
	air_man->send_str( air724, (char*)send_buf );	

	show_send_data( (char*)send_buf );

}
/*-----------------------------------------------------------*/
//���������¼����ĸ�������
void event_report_net( event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	
	memset( send_buf,0,sizeof(send_buf) );
	
	//��䷢�� ������ �ϱ������������
	//�� LOG 295 alarm  01 guzhang 1234567890123456
	//LOG 295 listname event subval 1234567890123456
	//LOG 295 ������ �¼� ���� ������
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"LOG " );
	dump_len += ret;
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),get_msh_terminal_str() );
	dump_len += ret;
	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 14" );
	dump_len += ret;
	switch( event->num )
	{
		case 0:	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 01" );break;
		case 1:	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 02" );break;
		default: 
						ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00" );break;
	}
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00 " );
	dump_len += ret;

	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),event->ddh );
	dump_len += ret;
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"\r\n" );
	dump_len += ret;
	
	//����ACK
//	air_man->send_str( air724, (char*)send_buf );	

	show_send_data( (char*)send_buf );

}
/*-----------------------------------------------------------*/
//���͹�����¼��������
void event_report_record( event_pt event )
{
	int ret = 0;
	int dump_len = 0;
	
	memset( send_buf,0,sizeof(send_buf) );
	
	//��䷢�� ������ �ϱ������������
	//�� LOG 295 alarm  01 guzhang 1234567890123456
	//LOG 295 listname event subval 1234567890123456
	//LOG 295 ������ �¼� ���� ������
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"LOG " );
	dump_len += ret;
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),get_msh_terminal_str() );
	dump_len += ret;
	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 15" );
	dump_len += ret;
	
	
	switch( event->val )
	{
		case 1:	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 01" );break;
		case 2:	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 02" );break;
		default: 
						ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00" );break;
	}
	dump_len += ret;
	
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," 00 " );
	dump_len += ret;

	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),event->ddh );
	dump_len += ret;
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"\r\n" );
	dump_len += ret;
	
	//����ACK
//	air_man->send_str( air724, (char*)send_buf );	

	show_send_data( (char*)send_buf );
	
}

