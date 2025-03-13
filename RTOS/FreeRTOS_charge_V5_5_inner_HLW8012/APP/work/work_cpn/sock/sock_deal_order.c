#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	



/*---------------------------------------------------------------------*/
//����������
void cycle_wait_ordef_req( int id )
{
	int i = 0;
	int ret = 0;
	sock_sta_pt pssock = &(socks[id]);
	if( 1 == pssock->order_remind )
	{
		pssock->order_remind = 0;
		switch( pssock->order.cmd )
		{
			//��綩��
			case oct_start_charge:
				sysprt->talog(" id=%02d:oct_start_charge [%010u] \r\n",id,get_sys_ticks());
				
				if( guide_dat->order_info->sock[id].working == __OFF )
				{//������˵��û�й����еĶ���, ��������
					PRT_LOG( "[A] no working order,start charge\r\n" );
					pssock->ssta = 1;		//�л�״̬���� cycle_work()�������á�
					
					//���ƶ�����
					for( i=0;i<MaxDDH_Len;i++ )
						guide_dat->order_info->sock[id].ddh[i] = pssock->order.ddh[i];
					
					guide_dat->order_info->sock[id].start = rtc_dt;		
					
					//�������ʱ��
					guide_dat->order_info->sock[id].end   = rtc_dt;		
					addTime( &(guide_dat->order_info->sock[id].end),0,0,0,0,pssock->order.dat,0 );
					
					//���������ܼ���
					for( i=0;i<MaxSolt;i++ )
						guide_dat->order_info->sock[id].pluse[i] = 0;
					
					//��ǹ�����
					guide_dat->order_info->sock[id].working = __ON;
					
					//��2��ʼ������
					guide_dat->rtd_num->sock[id] =  2;
					
					//�洢������Ϣ
					wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
					
					//�洢�����Ϣ
					wdat_opt->rtd_num.set_to_fdb( guide_dat->rtd_num );
					PRT_LOG("guide_dat->rtd_num->sock[id] = %d\r\n",guide_dat->rtd_num->sock[id]);
					
					//��¼:������ʼʱ��
					pssock->otick_order = get_sys_ticks();
					
					//��¼:�õ������ܵ�ʱ��
					uint32_t stime = datetime_to_minutes( guide_dat->order_info->sock[id].start );
					uint32_t etime = datetime_to_minutes( guide_dat->order_info->sock[id].end );
					pssock->atime_order = etime - stime;	

					pssock->average_pwr = 0;
					pssock->average_curr = 0;
					pssock->cntr_out = 0;

					//��¼: ����û�н�������
					pssock->order_stop_req = 0;
				}
				else
				{//����˵���ж����ڹ�����
					
					
					ret = strlib->strcmpn( guide_dat->order_info->sock[id].ddh,pssock->order.ddh,strlen(pssock->order.ddh) );
					if( 0 == ret )
					{//˵�������Ų�һ��,���ܸ��ĵ�ǰ����,��Ϊ������ܺ��п��ܱ���յ�,������������������
						PRT_LOG( "[B] has working order,ddh is not same,no to do\r\n" );
						sysprt->aerr( "Err:ddh is no same,can't change sock[%d],ddh err \r\n",id);
						return;
					}
					else
					{//˵����������ͬ,��������Ҫ�޸Ķ���
						PRT_LOG( "[C] has working order,ddh is same, change time\r\n" );
						pssock->ssta = 1;		//�л�״̬���� cycle_work()�������á�
						
						//�������ʱ��
						guide_dat->order_info->sock[id].end   = rtc_dt;	
						addTime( &(guide_dat->order_info->sock[id].end),0,0,0,0,pssock->order.dat,0 );
						
						
						
						//�洢������Ϣ
						wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
						
						//��¼:������ʼʱ��
						pssock->otick_order = get_sys_ticks();			
						
						//��¼:�õ������ܵ�ʱ��
						uint32_t stime = datetime_to_minutes( guide_dat->order_info->sock[id].start );
						uint32_t etime = datetime_to_minutes( guide_dat->order_info->sock[id].end );
						pssock->atime_order = etime - stime;

						//��¼: ����û�н�������
						pssock->order_stop_req = 0;
						
						pssock->average_pwr = 0;
						pssock->average_curr = 0;
						pssock->cntr_out = 0;
						
					}
					
				}
				break;
				
			//ȡ�����	
			case oct_cancel_charge://
//				pssock->dtick_order = get_sys_ticks();
				
//				//�������ʱ��
//				guide_dat->order_info->sock[id].end   = rtc_dt;		//��ǰʱ����ڽ���ʱ��	
//				
//				//�洢������Ϣ
//				wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
				//��¼: ����û�н�������
				pssock->order_stop_req = 1;	
				
				break;
			
			default:
				break;
		}
	}
}
/*---------------------------------------------------------------------*/
//�ȴ����綩��
void wait_net_order( void )
{
	order_t torder;
	sock_sta_pt pssock;
	uint8_t id = 0;
	int i = 0;
//	int ret = 0;
	if( sock_mq->recv_order( &torder ) >= 0  )
	{
		
		sysprt->aerr( "recv_order :\r\n");
		sysprt->aerr( "\tddh   :%s\r\n",torder.ddh );
		sysprt->aerr( "\tnum   :%d\r\n",torder.num );
		sysprt->aerr( "\tcmd   :%d\r\n",torder.cmd );
		sysprt->aerr( "\tdat   :%d\r\n",torder.dat );
		sysprt->aerr( "\tparam :%d\r\n",torder.param );
		
		//�ն˺�ת����id
		id = torder.num ;
		
		if( id >= MaxSock )
		{
			sysprt->aerr( "recv_order id(%d) > MaxSock \r\n",id );
			return;
		}
		pssock = &(socks[id]);
		
		//�ȿ��Ƿ�Ϊ�������������������Ĳ��ǵ�ǰ�������������
		if( torder.cmd == oct_cancel_charge  )
		{
			if( 0 == strlib->strcmpn( pssock->order.ddh,torder.ddh,strlen( torder.ddh )  ) )
			{//˵�������Ų�һ��,���ܽ�����ǰ����
				sysprt->aerr( "Err:oct_cancel_charge sock[%d],ddh err \r\n",id );
				return;
			}
		}
		//��id���ж����ַ�
		
		pssock->order_remind = 1;
		sysprt->aerr( "recv_order id->%d \r\n",id );
		for( i=0;i<MaxDDH_Len;i++ )
			pssock->order.ddh[i] = torder.ddh[i];
		
		pssock->order.num = torder.num;
		pssock->order.cmd = torder.cmd;
		pssock->order.dat = torder.dat;
		pssock->order.param = torder.param; 
		
	}
}

