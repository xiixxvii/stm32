
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)//dprintf(X)	

/*--------------------------------------------------------------------------------*/
//�����������

extern int rp_serv_first_boot;		//�����Ƿ���Ҫ���߷�������һ������

static void report_serv_update_success(void)
{
	char *ddh = NULL;
	time_t tick = 0;
	PRT_LOG("======>report_serv_update_success\r\n");
	ddh = mem_alloc( MaxDDH_Len );
	
			
	/*--------------------------------------------------------------------------------*/
	tick = get_sys_ticks();
	//�鿴�Ƿ���Ҫ֪ͨ���������ο����ɹ�
	if( rp_serv_first_boot )
	{
		//����(�¼���+i+tick)%8�ķ�ʽ���ɶ����ţ��ö����ſ������������
		for( int i=0;i<MaxDDH_Len;i++ )	ddh[i] = ( (_event_m_tenance +i+tick)&7 )+'0';	
		ddh[MaxDDH_Len-1] = '\0';
		
		sock_mq->put_event( ddh,_event_m_tenance,0x01,0x00,0,0,PNULL );
	}
	/*--------------------------------------------------------------------------------*/

	mem_free( ddh );
	
	
}


/*------------------------------------------------------------------*/
//
static void report_serv_net_reconnect(void)
{
	char *ddh = NULL;
	time_t tick = 0;
	
	ddh = mem_alloc( 20 );
/*--------------------------------------------------------------------------------*/
	//�����������������ǹصģ������󣬷�����Ϣ�������������ڽ����쳣�ϵ絼�µĶ�����
	//TODO_START:

	tick = get_sys_ticks();

	for( int id=0;id<MaxSock;id++ )
	{
		//���ڳ��Ͳ��ù�
		if( cat_sock_working( id ) ) continue;
		
		//������������ڹ����У�����Ҫ�ϱ�һ��
		PRT_LOG( "report_serv_net_reconnect:sock[%d] is stop,report to serv\r\n",id+1 );
		
		//�������ʧ�ܣ���˹��ܾͲ�����
		if( ddh == NULL ) break; 
		
		//����(id+i+tick)%8�ķ�ʽ���ɶ����ţ��ö����ſ������������
		ddh[3]= Brd_Version+'0';
		ddh[2]= Brd_SubVersion+'0';
		ddh[1]= App_Version+'0';
		ddh[0]= App_SubVersion+'0';
		
		for( int i=4;i<20;i++ )	ddh[i] = ( (id+i+tick)&7 )+'0';	
		ddh[16] = 0; ddh[17] = '\0';
		
		//�����¼�,����id֪�����ĸ�����
		sock_mq->put_event( ddh,_event_net,id,0,0,0,PNULL );

		
		
	}
		//TODO_END	

	mem_free( ddh );
}

static uint8_t rp_net_sta = 0;
static time_t otick_rp = 0;
static time_t ntick_rp = 0;


//�������������ı�־��λ:�ɹ���������Ҫ��λ״̬
static void rp_net_sta_rst(void)
{
	rp_net_sta = 0;
	
}

static void rp_work( void )
{
	switch( rp_net_sta )
	{
		case 0:
			otick_rp = get_sys_ticks();
			rp_net_sta ++;
			PRT_LOG("======>rp_work: -->%d,[%010u]\r\n",rp_net_sta,otick_rp );

			break;
		
		case 1:
			ntick_rp = get_sys_ticks();
			if( ( ntick_rp - otick_rp ) <(20* 1000 ) ) break;
			
			
			//����20���Ӻ󣬼���������������߷�����
			report_serv_net_reconnect();
			
			rp_net_sta++;
			PRT_LOG("======>rp_work: -->%d,[%010u]\r\n",rp_net_sta,ntick_rp );
			break;
		
		case 2://��ģ�鹤���Ѿ����
			break;
		
		default:
			break;
	}
}
/*--------------------------------------------------------------------------------*/
//��ȡ��ʱʱ��
static void get_slot( void )
{
	static uint32_t otick_slot = 0;
	uint32_t ntick =0;

	if( guide_dat->time_slot->get_slot != 0 ) return;
	
	ntick = get_sys_ticks();
	if( ( ntick - otick_slot ) <= 5000 ) return;
	otick_slot = ntick;
	
	//��ʱ����
	air_man->send_str( air724, "GETJFPG\r\n" );	
	
	show_send_data( "GETJFPG\r\n" );
}

/*--------------------------------------------------------------------------------*/
//��ʱͬ������ʱ��
static void get_net_time( void )
{
	static uint32_t otick_slot = 0;
	uint32_t ntick =0;
	
	ntick = get_sys_ticks();
	if( ( ntick - otick_slot ) <= (10*60*1000) ) return;
	otick_slot = ntick;
	
	//��ʱ����
	air_man->send_str( air724, "GETTIME\r\n" );	
	
	show_send_data( "GETTIME\r\n" );
}


/*--------------------------------------------------------------------------------*/

//��ѯ��ҵ�����ղ���������
void work_internet_recv_analysis( wk_inet_pt inet )
{
//	char *ddh = NULL;
//	time_t tick = 0;
	switch( inet->ssta )
	{
		//
		case 0://��ʼ����������
			PRT_LOG("work_internet_recv_analysis \r\n");
			//���ջ�������λ
			wira_work_data_rst();
			
			//����ǵ�һ����������֪ͨ�����������³ɹ�
			report_serv_update_success();
			
			//��λ������������״̬��,�Ժ�ʮ����������״̬��
			//��������ǹصģ�����߷�����
//			rp_net_sta_rst();
			
			nwdt->feed( inet ); //ι��
			inet->ssta++;
			
			break;
		
		case 1://�����������
		
			//��ʱ���ƴ���
			if ( nwdt->deal( inet )<0 ) break;

			//��ȡ��ʱʱ��
			get_slot();

			//��ʱͬ������ʱ��
			get_net_time();
		
			//�����ɹ���Ҫ֪ͨ������
//			rp_work();
		
			//�������ݲ�����
			wira_wrok_cycle( inet );
		
			//Ӳ���¼��ϱ� herp
			wira_event_report();
			
			//�����¼�����
			ipool->work();
			
		
			break;
		
		default:
			break;
	}
	

}





