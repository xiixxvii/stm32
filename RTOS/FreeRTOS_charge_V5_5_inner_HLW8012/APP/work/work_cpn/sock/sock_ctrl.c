
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	










/*---------------------------------------------------------------------*/
//Ӳ������
//�û����ۿ���ȥ�������1���ұ���2
//����Ա���У������0���ұ���1
//��Ӧ��Ӳ��������� IO_ELC1��IO_ELC0��(��ʵ���֪����)
static void open( int road )
{
	sysprt->alog( " open ( %d) \r\n",road );
	switch( road )
	{

		case 0:gpio->write( IO_ELC1,1 );break;
		case 1:gpio->write( IO_ELC0,1 );break;
		default:
			break;
	}
}
static void close( int road )
{
	sysprt->alog( " close ( %d) \r\n",road );
	switch( road )
	{

		case 0:gpio->write( IO_ELC1,0 );break;
		case 1:gpio->write( IO_ELC0,0 );break;

		default:
			break;
	}
}


/*---------------------------------------------------------------------*/

static time_t otick_ch[MaxSock] = {0,0};
static void rst_check_hight( int id )
{
	otick_ch[id] = get_sys_ticks();
	coder->clr_val( id );
}

//����1���������
int check_hight_event(int id,uint32_t tick)
{
	int ret = 0;
	uint32_t val = 0;
	if( (tick - otick_ch[id]) < (5*1000) ) return 0;
	otick_ch[id] = tick;
#if ( BoardVision == BoardV5_4)
	//�ɰ��Ӳ����ڸߵ���Ԥ���ź�,ֱ�ӷ���0
	ret = 0;
#endif
	
#if ( BoardVision == BoardV5_5)
	val = coder->cat_val( id );
	if(  val >= 100 ) 
	{
		ret =1;
		PRT_LOG("check_hight_event(%d) = %d \r\n",id,coder->cat_val( id ) );
		coder->clr_val( id );
	}
#endif
	return ret;
}





/*---------------------------------------------------------------------*/
//������ʾ��ˢ��

static void display( int id )
{
	uint32_t val_on = 0;			//����Ӧ�������ŵ�
	
	uint64_t ntick = get_sys_ticks();
	sock_sta_pt pssock = &(socks[id]);
	
	//��ʱ1sˢ��
	static time_t otick_disp[ MaxSock ];
	
	if( ( ntick - otick_disp[id] )<1000 ) return;
	otick_disp[ id ] = ntick;
	
	utime_refresh( pssock,ntick);
	
	//��üƻ����ʱ��
 	uint32_t all_time = pssock->atime_order;

	//���ʵ�ʳ��ʱ��
	uint32_t con_time = pssock->utime_order;

	
	//�����ζ���ʱ����Ϊ7��(��Ϊ��7����)
	uint32_t temp = all_time / 7 ;																//������ʱ���Ϊ7��
	
	//������ʱ�� ռ����
	if( con_time >= all_time )
		val_on = 0;																									//��������������Ϸ������0��ʼ����7���ơ�
	else
		val_on = con_time / temp;
		
	led_board->mode_req( id,_e_mode_charging,val_on,NULL );	//
	
	//�ڳ������У���ӡ��������
//	if(  pssock->dtick_order > ntick )
//		sysprt->alog("display[%02d]: %010llu s led[%ld]->now:[%04d/%0.5fw]%.5fA\r\n",
//								id,(pssock->dtick_order - ntick)/1000,val_on,hlw8012->read_pul(id),sock_hlw_opt->cat_epwr(id),cat_curr( id ));
//	else
//		sysprt->alog("display[%02d]: %010llu->now:%.5f\r\n",id,0,cat_curr( id ) );
}
/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
#define read_curr_tick 1000											//��ȡ�����ĵδ�����
#define avg_curr_cntr	 5												//�����ٴε�������һ��ƽ��ֵ


static time_t otick_put_curr[MaxSock] = {0,0};								//���ڶ�ʱ��������������
static time_t otick_curr_diff[MaxSock] = {0,0};						//���ڶ�ʱ5s�жϵ����仯״̬

//static uint32_t rtd_cntr
//��ʱ������������ƽ��ֵ�����ϴ���������
static void cycle_listen_curr( int id )
{
	uint8_t com = id+1;
	rtc_dt_t *dt = 0;
	double diff_curr = 0;
	int report_curr = 0;
	sock_sta_pt pssock = &(socks[id]);
	
	time_t ntick = get_sys_ticks();									//������ǰϵͳ�δ�
	
	//ÿ��5s�ж�һ�ε���ֵ�Ƿ�仯�ϴ�( 20% ),����仯�ϴ�ֱ�Ӵ�һ��
	if( ( ntick - otick_curr_diff[id] ) > 5000 )
	{
		otick_curr_diff[id] = ntick;
		
		//��߳���׳��
		if( pssock->average_curr <0 ) pssock->average_curr = 0;
		if( pssock->base_curr <0 ) 		pssock->base_curr = 0;
		
		
		//���,�õ���ֵ
		if( pssock->average_curr > pssock->base_curr  )
			diff_curr = pssock->average_curr - pssock->base_curr;
		else
			diff_curr = pssock->base_curr - pssock->average_curr;
		
		report_curr = 0;
		//ǰ��40s��ֱ�Ӷ�ʱ5s�������������ں�ֱ̨�ӿ����û���û���õ���
		if( ( ntick - pssock->otick_order ) < 40000 ) report_curr ++;
		//��������£����������ٷ�
		if( ( diff_curr > ( pssock->base_curr * 0.2) )&&( pssock->average_curr >0.09  ) )
			report_curr++;
		
		//
		if( report_curr >0 )
		{//��һ�ε���
			goto __report_curr;
		}
		
		
	}
	
	//��ʱ ��һ�ε���ֵ ��������
//	if( ( ntick - otick_put_curr[id] ) > (wdat->app_dat.tick_put_curr) )
	if( ( ntick - otick_put_curr[id] ) > (15*1000) )	//��ʱ15s��һ�������������
	{
		dt = &(guide_dat->order_info->sock[id].start);
		prt_date_time( dt,"start  " );

		dt = &(guide_dat->order_info->sock[id].end);
		prt_date_time( dt,"end    " );
		goto __report_curr;

	}
	return;
	
__report_curr:

	PRT_LOG("guide_dat->rtd_num->sock[%d] = %d\r\n",id,guide_dat->rtd_num->sock[id]);
	sock_mq->put_event( pssock->order.ddh,_event_curr,com,0,guide_dat->rtd_num->sock[id],pssock->average_curr,PNULL );
	guide_dat->rtd_num->sock[id]++;

	//ÿ��ȡ16���������ݴ洢һ��
	if( 0 == ( guide_dat->rtd_num->sock[id] & 0xF ) )
		wdat_opt->rtd_num.set_to_fdb	( guide_dat->rtd_num);			//���
	
	//���»�������
	pssock->base_curr = pssock->average_curr;
	
	//��Ϊ�Ѿ�����һ�ε������Ƴ� ��ʱ ��һ�ε���ֵ �������� ��ʱ��
	otick_put_curr[id] = ntick;
	
	return;
	
}

static void recv_hlw_event( int id,int working )
{
	static uint32_t prt_cntr[MaxSock] = {0,0};
	
	sock_sta_pt pssock = &(socks[id]);
	
	hlw_pul_t hlw_event;
	double dval = 0;
	int segment = 0;
	int ret = sock_hlw_opt->get_cyc_pul( id,&hlw_event );
	if( ret <0 ) return;
	
	time_t ntick = get_sys_ticks();									//������ǰϵͳ�δ�
	
	//����ǹ�����,��������
	if( working == 0 ) return;
	
	
	switch( hlw_event.type )
	{
		//�ε�����
		case _type_hlw_sock_out:
			
			if( pssock->cntr_out == 0 )
			{//˵���յ��˲����͹����¼������ǻ���Ҫ��һ��ȷ��
				pssock->tick_out = ntick;
				pssock->cntr_out++;
			}
			else
			{//˵��
				if( ( ntick - pssock->tick_out ) < ( 10*1000 ) )
				{//10��֮�ڲ����ģ�˵����İγ���
					pssock->sock_out = 1;
				}
				else
				{
					pssock->tick_out = ntick;
					pssock->cntr_out++;
				}
			}
			
			
				
			break;
		
		//��������
		case _type_hlw_cyc_pul:
		
			if( hlw_event.val > hlw_event.cyc )
			{//˵���й�������
				pssock->cntr_out = 0;
			}
			
		
			//�ӵ��ܵ���ȥ
			guide_dat->total_e->pul[id] += hlw_event.val;
		
			//�õ�����һ��ʱ���
			segment = get_time_segments( guide_dat->time_slot->val,guide_dat->time_slot->segments,rtc_dt.Hour,rtc_dt.Minute );
			//�ѵ��ܼӵ�������Ϣȥ
			guide_dat->order_info->sock[id].pluse[ segment ] += hlw_event.val;
			
			//�õ��������ڵĵ������� W*s
			dval = hlw_event.val *  wdat->k[id] + wdat->b[id];
			//�õ������ڵ�ƽ������ W
			pssock->average_pwr = dval / hlw_event.cyc;
			pssock->average_curr = pssock->average_pwr/220;
			//�õ�ƽ��ÿ��������
			dval = hlw_event.val;
			dval = dval / hlw_event.cyc;
			//sysprt->talog("id=%02d,average_curr %f \r\n",id,average_curr[ id ] );
			sysprt->alog("==========================================\r\n");
			sysprt->alog("  [%.5f]---%8d\r\n",dval,prt_cntr[id]);
			sysprt->alog("  sock[%2d]curr = [%.5f]A \r\n",id,pssock->average_curr);
			sysprt->alog("  ����[%2d]���� = [%.5f]A \r\n",id,pssock->average_curr);
			sysprt->alog("  -->%.5f W [%d]\r\n",pssock->average_pwr,coder->cat_val( id ) );
			

			
			sysprt->alog("==========================================\r\n");
			prt_cntr[id]++;
			
			break;
		
		//һ��������(���ĵ���)
		case _type_hlw_min_pul:
			pssock->min_w_s = hlw_event.val *  wdat->k[id] + wdat->b[id];
			
			//�洢����	
			wdat_opt->total_e.set_to_fdb		( guide_dat->total_e);				//�ܵ���
			wdat_opt->order_info.set_to_fdb	( guide_dat->order_info);			//�������		
			
			break;
		
		default:
			break;
	}
}


enum {
	_e_wait_order=0,		//�ȴ�����
	_e_order_start,			//������ʼ
	_e_charging,				//�����
	
};

//ѭ����ҵ
static void cycle_work( int id )
{
	int group = 0;				//������������  ������һ����( JFPG )
	int segments;					//���������
	double kwh;						//���ڴ洢�õ���
	
	slot_t use_period;		//�õ�����
	
	slot_t usr_period[2];	//��Խ������
	
	uint32_t overlap=0;		//�ص�ʱ��
	
	uint32_t tmin0 =0,tmin1=0;
	
//	static time_t otick_cr[ MaxSock ];				//���ڿ��Ƶ���/���ʵķ���Ƶ��
	period_e_t period_e[MaxPeriodE];						//���ڴ洢��������ʱ���������
	
	
	sock_sta_pt pssock = &(socks[id]);
	

	time_t ntick = get_sys_ticks();									//������ǰϵͳ�δ�
//	double curr = 0.00;															//���ڴ洢��ǰ����
	switch( pssock->ssta )
	{
		case _e_wait_order://�ȴ�����
			//�ڴ˴�ʲô�����������յ���������޸�ssta[id]��ֵ
			recv_hlw_event( id,0 );
			
			break;
		case _e_order_start://��ʼ����
			open( id );						//��ͨ����
			rst_check_hight( id );		//��ոߵ������
			pssock->ssta++;						//
			break;
		case _e_charging://����� 1
			//-----------------------------------------------------
			//��ʱ������������ƽ��ֵ�����ϴ���������
			cycle_listen_curr(id);
			
			recv_hlw_event( id,1 );
			//-----------------------------------------------------
			//����Ƿ�������
			if( (ntick - pssock->otick_order)>(40*1000) )
			{
				if( pssock->sock_out )
				{
					sysprt->alog("sock[%d] oeb_no_ins \r\n");
					//�������ζ���
					pssock->ssta = 0xE0+oeb_no_ins;	
					//������һ�������������
					
					sock_mq->put_event( pssock->order.ddh,_event_recore,id,oeb_no_ins,0,0,PNULL );
					break;
				}
			}
			else
			{
				pssock->sock_out = 0;	//ǰ��40s�Ͳ�������״̬��
			}
		
		
			//-----------------------------------------------------
			//10�� ��������Ƿ��������������������
			//���ʹ�����ϵ�����

			if( (1 == check_hight_event( id,ntick ) )||( pssock->average_pwr >= 900 ) )
			{
				
				
				sysprt->alog("sock[%d] oeb_curr_too_big \r\n");
				//�������ζ���
				pssock->ssta = 0xE0+oeb_curr_too_big;	
				//������һ�������������
				sock_mq->put_event( pssock->order.ddh,_event_recore,id,oeb_curr_too_big,0,0,PNULL );
				break;
			}
			
			//-----------------------------------------------------
			//�����������Ƿ��С 
			if( (ntick - pssock->otick_order)>(120*1000) )
				if( pssock->min_w_s < (15*60)  )
				{
					sysprt->alog("sock[%d] min_w_s is to small->%f\r\n",pssock->min_w_s);
					//����ȷʵ̫С
					pssock->ssta = 0xE0+oeb_curr_too_small;	
					//������һ�������������
					sock_mq->put_event( pssock->order.ddh,_event_recore,id,oeb_curr_too_small,0,0,PNULL );
					break;
				}
			
			//-----------------------------------------------------
			//��Ᵽ��˿�Ƿ��ջ�
			if( 1 == cat_fuse_err(  id ) )
			{//����˿�𻵣�����
				pssock->ssta = 0xE0+oeb_fuse_broken;
				
			}
			//-----------------------------------------------------
			//��ⶩ����ʱ
			
			tmin0 = datetime_to_minutes( guide_dat->order_info->sock[id].end );
			tmin1 = datetime_to_minutes( rtc_dt );
			if( tmin1>= tmin0  )
//			if( ntick >= pssock->dtick_order )
			{//����,˵����������
				pssock->ssta = 0xE0+oeb_time;
				break;
			}
			//-----------------------------------------------------
			//���ڶ�����������
			if( pssock->order_stop_req )
			{//
				pssock->order_stop_req = 0;
				pssock->ssta = 0xE0+oeb_manual_stop;
				break;
			}
			
			//-----------------------------------------------------
			//��ʱ ˢ����ʾ
			display( id );
			break;
		//----------------------------------------------------
		case 0xE0+oeb_time:									//���ʱ��ľ�����
		case 0xE0+oeb_curr_too_small:				//�����̵����쳣(�ε�/����)		
		case 0xE0+oeb_curr_too_big:					//�����̵����쳣(�ε�/����)		
		case 0xE0+oeb_no_ins:								//�����̲������ε�
		case 0xE0+oeb_fuse_broken:					//�����̱���˿�쳣
		case 0xE0+oeb_manual_stop:					//�ֶ�������������ȴ���������
			sysprt->taerr("id=%02d,event:%02d \r\n",id,pssock->ssta-0XE0 );
			
			//�رղ���
			close( id );											
			
		
		
			//��������õ����
			for( int i=0;i<MaxPeriodE;i++ ) 
			{
				period_e[i].kwh = 0;
				period_e[i].min = 0;
			}
			group = 0;
			segments = guide_dat->time_slot->segments;
			kwh = 0;
			for( int seg=0; seg < segments; seg++ )
			{
				group = guide_dat->time_slot->val[ seg ].group;
				if( group < MaxPeriodE )
				{
					kwh = guide_dat->order_info->sock[id].pluse[ seg ];
					kwh = kwh * wdat->k[id] / 3600000; // ����/60��/60��/1000W
					period_e[ group ].kwh += kwh ;
				}
			}
			
			//�õ�������ֹʱ��
			guide_dat->order_info->sock[id].end 		= rtc_dt;
			//��Ƕ����Ѿ�����
			guide_dat->order_info->sock[id].working = __OFF;

			//����ʱ����ص�ʱ��
			
#if 1
			use_period.start = guide_dat->order_info->sock[id].start.Hour*60 + guide_dat->order_info->sock[id].start.Minute;
			use_period.end   = guide_dat->order_info->sock[id].end.Hour*60 + guide_dat->order_info->sock[id].end.Minute;

			//�����ǰ���� �� �������ڲ�һ������˵��������Խ������
			if( guide_dat->order_info->sock[id].end.Date != guide_dat->order_info->sock[id].start.Date )
			{
				
				use_period.end += 24* 60;
			}
			
			for (int seg = 0; seg < segments; seg++) 
			{
        overlap = calculate_overlap( guide_dat->time_slot->val[ seg ],use_period );
        PRT_LOG("%02d:%02d - %02d:%02d: %dmin\r\n",
					guide_dat->time_slot->val[ seg ].start/60, 
					guide_dat->time_slot->val[ seg ].start%60,
          guide_dat->time_slot->val[ seg ].end/60, 
					guide_dat->time_slot->val[ seg ].end%60,
          overlap);
				
				group = guide_dat->time_slot->val[ seg ].group;
				period_e[ group ].min += overlap ;
			}
#else
			//��Խ������ʱ�����⴦��
			
			
#endif

			
			
			//�ύ���������¼�
			sock_mq->put_event( pssock->order.ddh,_event_sock,	id,pssock->ssta - 0xE0 ,0,0,period_e );
			
			//�ı��ܵ�״̬Ϊ������״̬
			led_board->mode_req( id,_e_mode_standing,NULL,NULL );
			
			//�洢����״̬
			wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
			
			pssock->ssta = 0;
			break;
			
		default:
			break;
	}
}




/*---------------------------------------------------------------------*/
extern void run_deal_last_shutdown(void);

enum{
	_e_ctrl_init = 0,
	_e_wait_get_date,
	_e_cycle_work,
};

//������������
static void ctrl_one_sock( int id )
{
	int i=0;
	uint32_t stime = 0,etime=0,dtime=0;
	sock_sta_pt pssock = &(socks[id]);
	switch( pssock->msta )
	{
		case _e_ctrl_init://��ʼ��
			
			close( id );
			pssock->otick_order = 0;		
			pssock->order_remind = 0;
	
			pssock->msta++;
			pssock->ssta = 0;
			
			break;
		
		case _e_wait_get_date://�ȴ����ں�ʱ�λ�ȡ�ɹ�������δ��������
			if( ( 0 == guide_dat->time_slot->get_slot ) &&( 0 != wdat->net_mode )) break;
			
			//�������δ���������������ɶ�������������
			if( guide_dat->order_info->sock[id].working == __ON )
			{//������˵�����ڶ���δ��������������
				
				PRT_LOG("Sock[%d]has working not over,auto dump order\r\n",id);
				
				prt_date_time(&rtc_dt,"rtc_dt");
				prt_date_time(&(guide_dat->order_info->sock[id].end),"end   ");
				
				stime = datetime_to_minutes( rtc_dt );
				etime = datetime_to_minutes( guide_dat->order_info->sock[id].end );
//				PRT_LOG( "" )
				if( etime>= stime ) dtime = etime - stime;
				else dtime = 0;
//				sock_mq->send_order( guide_dat->order_info->sock[id].ddh,id,oct_start_charge,etime-stime,0 );
				sock_mq->send_order( guide_dat->order_info->sock[id].ddh,id,oct_start_charge,dtime,0 );
				
			}
			
			pssock->msta++;
			pssock->ssta = 0;
			
			break;
		
		case _e_cycle_work://��ѯ��ҵ
			
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
	sock_sta_pt pssock = &(socks[id]);
	
	if( (pssock->msta == _e_cycle_work)&&( pssock->ssta == _e_charging ) )
		return 1;
	else
		return 0;
}

//��ȡ����״̬
int get_sock_sta( int road )
{
	int ret = 0;
	if( 0 == cat_fuse_err( road ) )
	{//������˵������˿û�յ����ϴ�˵�������Ƿ����ڳ����
		if( cat_sock_working( road ) )
		{//������˵�����ڳ����
			ret = 0x02;
		}
		else
		{//�������ú���
			ret = 0x00;
		}
	}
	else
	{//����˵������˿�յ��ˡ��ϴ�˵����
		ret = 0x40;
	}
	return ret;
}

/*---------------------------------------------------------------------*/



//������������
void sock_ctrl_work( void )
{
	for( int id=0;id<MaxSock;id++ )
	{
		//����һ������
		ctrl_one_sock( id );
	}
	
}


