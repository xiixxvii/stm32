
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)//dprintf(X)	

//----------------------------------------
//�����������



//----------------------------------------
//��ѯ��ҵ�����ղ���������
void work_internet_recv_analysis( wk_inet_pt inet )
{
	char *ddh = NULL;
	time_t tick = 0;
	switch( inet->ssta )
	{
		//
		case 0://��ʼ����������
			PRT_LOG("work_internet_recv_analysis \r\n");
			//���ջ�������λ
			wira_work_data_rst();
			
			//�����������������ǹصģ������󣬷�����Ϣ�������������ڽ����쳣�ϵ絼�µĶ�����
			//TODO_START:

			tick = get_sys_ticks();
		
			for( int id=0;id<MaxSock;id++ )
			{
				//���ڳ��Ͳ��ù�
				if( cat_sock_working( id ) ) continue;
				
				//������������ڹ����У�����Ҫ�ϱ�һ��
				ddh = mem_alloc( 20 );
				
				//�������ʧ�ܣ���˹��ܾͲ�����
				if( ddh == NULL ) break; 
				
				//����(id+i+tick)%8�ķ�ʽ���ɶ����ţ��ö����ſ������������
				for( int i=0;i<20;i++ )	ddh[i] = ( (id+i+tick)&7 )+'0';	
				ddh[16] = 0; ddh[17] = '\0';
				
				//�����¼�,����id֪�����ĸ�����
				sock_mq->put_event( ddh,_event_net,id,0,0,0 );
			
				
				mem_free( ddh );
			}
			//TODO_END	
			
			
			nwdt->feed( nwdt,inet ); //ι��
			inet->ssta++;
			
			break;
		
		case 1://�����������
			//��ʱ���ƴ���
			if ( nwdt->deal( nwdt,inet )<0 ) break;

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





