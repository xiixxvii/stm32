
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	


extern sock_mq_pt sock_mq;
extern sock_hlw_opt_pt sock_hlw_opt;

static int msta[ MaxSock ];	//����2����������״̬

static int ssta[ MaxSock ];	//����2����������״̬

static int order_remind[ MaxSock ];	//�����Ƿ��ж�������
static order_t order[ MaxSock ];		//��¼��������
//static event_t event[ MaxSock ];		//



//��������ʱ����
static uint64_t otick_order[ MaxSock ];			//������ʼʱ��
//static time_t ntick_order[ MaxSock ];			//������ǰʱ��
static uint64_t dtick_order[ MaxSock ];			//����Ŀ��ʱ��


/*---------------------------------------------------------------------*/
//Ӳ������
static void open( int road )
{
	sysprt->alog( " open ( %d) \r\n",road );
	switch( road )
	{
		case 0:io_pin->out( 2, __H );break;
		case 1:io_pin->out( 1, __H );break;
		default:
			break;
	}
}
static void close( int road )
{
	sysprt->alog( " close ( %d) \r\n",road );
	switch( road )
	{
		case 0:io_pin->out( 2, __L );break;
		case 1:io_pin->out( 1, __L );break;
		default:
			break;
	}
}


static double cat_curr( int id )
{
	return sock_hlw_opt->cat_ecurr( id )  ;//curr_val[id];
}

/*---------------------------------------------------------------------*/
//����˿����
static int fuse_err = 0;

static int cat_fuse_err(void)
{
	return fuse_err;
}
static void listen_fuse( void )
{
	static time_t otick_fuse;
	static time_t ntick_fuse;
	static uint32_t cntr = 0;
	static int sta = 0;
	uint32_t val = 0;
	
	val = io_pin->in( 5 );
	
	switch( sta )
	{
		case 0://�ȴ�0
			if( 0 == val ) 
			{		
				sta++;
			}
			break;
		case 1://�ȴ�1
			if( 1 == val )
			{
				cntr ++;
				sta--;
			}
			break;
		default:
			break;
		
	}
	
	ntick_fuse = get_sys_ticks();
	
	if( 1000 >( ntick_fuse - otick_fuse ) )	return ;
	otick_fuse = ntick_fuse;
	
	
//	sysprt->talog("listen_fuse:%08d \r\n",cntr );
	
	if( cntr <40 )
	{	
		fuse_err = 1;
		led_board->set_fuse_led( __OFF );
		sysprt->talog("listen_fuse:%08d \r\n",cntr );
	}
	else
	{
		fuse_err = 0;
		led_board->set_fuse_led( __ON );
	}
	cntr = 0;
	return;
}
/*---------------------------------------------------------------------*/
//������ʾ��ˢ��

static void display( int id )
{
	uint32_t val_on = 0;			//����Ӧ�������ŵ�
	
	uint64_t ntick = get_sys_ticks();
	
	//��ʱ1sˢ��
	static time_t otick_disp[ MaxSock ];
	if( ( ntick - otick_disp[id] )<1000 ) return;
	otick_disp[ id ] = ntick;
	
	//��üƻ����ʱ��
	uint64_t all_tick = 0;
	all_tick = dtick_order[ id ] - otick_order[ id ];		
	
	//���ʵ�ʳ��ʱ��
	uint64_t con_tick = 0;
	if( ntick > otick_order[ id ] )
		con_tick = ntick - otick_order[id];								
	else
		con_tick = 0;			
	
	//�����ζ���ʱ����Ϊ7��(��Ϊ��7����)
	uint32_t temp = all_tick / 7 ;																//������ʱ���Ϊ7��
	
	//������ʱ�� ռ����
	if( con_tick >= all_tick )
		val_on = 0;																									//��������������Ϸ������0��ʼ����7���ơ�
	else		
		val_on = con_tick / temp;
		
	led_board->mode_req( id,_e_mode_charging,val_on,NULL );	//

	//�ڳ������У���ӡ��������
	if(  dtick_order[ id ] > ntick )
		sysprt->alog("display[%02d]: %.3f,%010llu led[%ld]\r\n",id,cat_curr( id ), dtick_order[ id ] - ntick,val_on);
	else
		sysprt->alog("display[%02d]: %.3f,%010llu \r\n",id,cat_curr( id ),0 );
}

/*---------------------------------------------------------------------*/
//ѭ����ҵ
static void cycle_work( int id )
{
	static uint32_t currs_err[ MaxSock ];			//����������С����Ĵ���	
	static uint32_t currb_err[ MaxSock ];			//���������������Ĵ���
	static uint32_t cnct_err[ MaxSock ];			//����δ���������Ĵ���
	static time_t otick_cr[ MaxSock ];				//���ڿ��Ƶ�������Ƶ��
	static time_t otick_ct[ MaxSock ];				//���ڲ������������Ƶ��
	
	static double scurr[ MaxSock ]={0,0};						//���ڿ���ƽ������
	static uint32_t scurr_cntr[ MaxSock ]={0,0};		//����ƽ������ ��ȡ����
	static time_t otick_scurr_cycle[ MaxSock ];			//���ڿ���ƽ��������������
	static time_t otick_scurr_read[ MaxSock ];			//���ڿ���ƽ��������ȡ����
	
	
	time_t ntick = get_sys_ticks();						//������ǰϵͳ�δ�
	double curr = 0.00;												//���ڴ洢��ǰ����
	switch( ssta[ id ] )
	{
		case 0://�ȴ�����
			//�ڴ˴�ʲô�����������յ���������޸�ssta[id]��ֵ
			
			break;
		case 1://��ʼ����
			open( id );						//��ͨ����
			currs_err[ id ] = 0;	//
			currb_err[ id ] = 0;
			cnct_err[ id ] = 0;		//
		
			scurr[ id ] = 0;			//��ʼ����ʱ����������
			scurr_cntr[ id ] = 0 ;
			otick_scurr_cycle[ id ] = get_sys_ticks();
			otick_scurr_read [ id ] = otick_scurr_cycle[ id ];
			
			ssta[id]++;						//
			break;
		case 2://����� 1
			
			//��ʱ��ȡ����¼
			if( ( ntick - otick_scurr_read[ id ] ) >= 1000 )
			{
				otick_scurr_read[id] = ntick;
				
				scurr[id] += cat_curr( id );
				scurr_cntr[id]++;
			}
			
			//��ʱ�ϴ�
			if( ( ntick - otick_scurr_cycle[ id ] ) >= (5*1000) )//��λms
			{
				otick_scurr_cycle[id] = ntick;
				
				curr = scurr[id] / scurr_cntr[id];
				
				//������ֵ
				if( curr >5.00 ) curr = 5.00;
				
				//�������������
				sock_mq->put_event( order[id].ddh,_event_curr,id,NULL,0,curr );
				
				//���ö�ȡ
				scurr[id] = 0;
				scurr_cntr[id] = 0;
				otick_scurr_read[id] = ntick;
			}
			
			//-----------------------------------------------------
			//����Ƿ�������
#if 0
			if( ( ntick - otick_ct[ id ] ) >= 2000 )
			{
				otick_ct[id] = ntick;
				
				if( 0 ==  sock_hlw_opt->cat_connect( id ) )
				{//���μ�⵽û�в����ͷ
					cnct_err[ id ]++;
					sysprt->talog("id=%02d,cat_connect %d \r\n",id,cnct_err[ id ] );
					if( cnct_err[ id ] >= 10 )
					{//ȷʵû�в����ͷ
						ssta[ id ] = 0xE0+oeb_no_ins;
						break;
					}
				}
				else{	cnct_err[ id ] = 0;	}
			}
#endif
			//-----------------------------------------------------

			//������
#if 1
			if( ( ntick - otick_cr[ id ] ) >= 100 )
			{
				otick_cr[id] = ntick;
				curr = cat_curr( id );
				
				
				
				if( curr < 0.1 )
				{//���μ�������С
					currs_err[ id ]++;
					sysprt->talog("id=%02d,%fA curr is to small:%02d \r\n",id,curr,currs_err[ id ] );
					if( currs_err[ id ] >= (180*10) ) 		//10����1s 3���Ӿ���180s
					{//�������ȷʵ̫С
						ssta[ id ] = 0xE0+oeb_curr_too_small;	
						//�������������
						sock_mq->put_event( order[id].ddh,_event_recore,oeb_curr_too_small,NULL,0,0 );
						break;

					}
				}
				else
				{
					if( curr > 4.0 )
					{//���μ���������
						currb_err[ id ]++;
						sysprt->talog("id=%02d,%fA curr is to big:%02d \r\n",id,curr,currb_err[ id ] );
						if( currb_err[ id ] >= (60*10) )			//10����1s 3���Ӿ���180s
						{//�������ȷʵ̫����
//							ssta[ id ] = 0xE0+oeb_curr_too_big;	
//							break;
							//�������������
							sock_mq->put_event( order[id].ddh,_event_recore,oeb_curr_too_big,NULL,0,0 );
						}
					}
					else
					{//��������
						currs_err[ id ] = 0;
						currb_err[ id ] = 0;
					}
					
				}
			}
#endif
			//-----------------------------------------------------
			//��Ᵽ��˿�Ƿ��ջ�
			if( 1 == cat_fuse_err() )
			{//����˿�𻵣�����
				ssta[ id ] = 0xE3;
				
			}
			//-----------------------------------------------------
			//��ⶩ����ʱ
			if( ntick >= dtick_order[ id ] )
			{
				ssta[ id ] = 0xE0;
				break;
			}
			//-----------------------------------------------------
			//��ʱ ˢ����ʾ
			display( id );
			break;
		//----------------------------------------------------
		case 0xE0+oeb_time:					//���ʱ��ľ�����
		case 0xE0+oeb_curr_too_small:				//�����̵����쳣(�ε�/����)		
		case 0xE0+oeb_curr_too_big:					//�����̵����쳣(�ε�/����)		
		case 0xE0+oeb_no_ins:								//�����̲������ε�
		case 0xE0+oeb_fuse_broken:					//�����̱���˿�쳣
			sysprt->taerr("id=%02d,event:%02d \r\n",id,ssta[id]-0XE0 );
			
			//�رղ���
			close( id );											
			
			//�ύ�¼�
			sock_mq->put_event( order[id].ddh,_event_sock,	id,ssta[id] - 0xE0 ,0,0 );
		
			//�ı��ܵ�״̬Ϊ������״̬
			led_board->mode_req( id,_e_mode_standing,NULL,NULL );
			ssta[ id ] = 0;
			break;
			
		default:
			break;
	}
}



/*---------------------------------------------------------------------*/
//����������
static void cycle_wait_ordef_req( int id )
{
	
	if( 1 == order_remind[id] )
	{
		order_remind[id] = 0;
		switch( order[id].cmd )
		{
			//��綩��
			case oct_start_charge:
				sysprt->talog(" id=%02d:oct_start_charge [%08d] \r\n",id,get_sys_ticks());
				//��ȡ������ʼʱ��	
				
				ssta[ id ] = 1;		//�л�״̬���� cycle_work()�������á�
			
				//��¼:������ʼʱ��
				otick_order[id] = get_sys_ticks();			
				
				//��¼:��������ʱ��
				dtick_order[id] = otick_order[id] + order[id].dat;
				break;
		
			//ȡ�����	
			case oct_cancel_charge://
				dtick_order[id] = get_sys_ticks();
				break;
			
			default:
				break;
		}
	}
}
/*---------------------------------------------------------------------*/

//������������
static void ctrl_one_sock( int id )
{
	switch( msta[ id ] )
	{
		case 0://��ʼ��
			
			close( id );
			otick_order[id] = 0;		
			order_remind[id] = 0;
		
			msta[ id ]++;
			ssta[ id ] = 0;
			break;
		
		case 1://��ѯ��ҵ
			
			cycle_wait_ordef_req( id );
			
			cycle_work( id );
				
			break;
			
		default:
			break;
	}
	
}
/*---------------------------------------------------------------------*/

//��ѯ�����Ƿ����ڳ��
int cat_sock_working( int id )
{
	if( (msta[id] == 1)&&( ssta[id] == 2 ) )
		return 1;
	else
		return 0;
}

/*---------------------------------------------------------------------*/

static void wait_order( void )
{
	order_t torder;
	uint8_t id = 0;
	int i = 0;
	if( sock_mq->recv_order( &torder ) >= 0  )
	{
		
		sysprt->aerr( "recv_order :\r\n");
		sysprt->aerr( "torder.ddh :%s\r\n",torder.ddh );
		sysprt->aerr( "torder.num :%d\r\n",torder.num );
		sysprt->aerr( "torder.cmd :%d\r\n",torder.cmd );
		sysprt->aerr( "torder.dat :%d\r\n",torder.dat );
		
		//�ն˺�ת����id
		id = torder.num - 1;
		if( id > MaxSock )
		{
			sysprt->aerr( "recv_order id(%d) > MaxSock \r\n",id );
			return;
		}
		//��id���ж����ַ�
		order_remind[ id ] = 1;
		sysprt->aerr( "recv_order id->%02 \r\n",id );
		for( i=0;i<20;i++ )
			order[ id ].ddh[i] = torder.ddh[i];
		
		order[ id ].num = torder.num;
		order[ id ].cmd = torder.cmd;
		order[ id ].dat = torder.dat;
		
	}
}

void sock_check_service_work( void )
{
	
}

void sock_ctrl_work( void )
{
	
	//��������˿״̬
	listen_fuse();
	
	//�ȴ�����ָ��
	wait_order();

	//��������״̬
	sock_hlw_opt->work();
	
	for( int id=0;id<MaxSock;id++ )
	{
		//����һ������
		ctrl_one_sock( id );
	}
}

