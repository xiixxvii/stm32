#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	


//����֮ǰ�����ô˳���
void save_order_them_shutdown( void )
{
//	int change = 0;

//	//����Ƿ��в������ڶ���
//	time_t now_tick = get_sys_ticks();
//	for( int id = 0;id<MaxSock;id++ )
//	{
//		if( cat_sock_working( id ) )
//		{//˵�����ڳ��
//			PRT_LOG("[%d] create restart_order \r\n",id);
//			
//			//���湤��״̬
//			wdat->last_order[id].work_code = WorkIngCode;
//			//���涩��ʣ��ʱ��
//			wdat->last_order[id].tick = socks[id].dtick_order - now_tick;//socks[id].otick_order;	
//			
//			//���涩����
//			for( int i=0;i<20;i++ )
//			{
//				wdat->last_order[id].ddh[i] = socks[id].order.ddh[id];
//			}
//			
//			PRT_LOG("\t tick = %d\r\n",wdat->last_order[id].tick);
//			PRT_LOG("\t ddh  = %s\r\n",wdat->last_order[id].ddh);
//			
//			
//			change ++;
//		}
//	}
//	
//	if( change > 0 )
//	{//˵����Ҫ��������
//		work_data_write();
//	
//	}
}




