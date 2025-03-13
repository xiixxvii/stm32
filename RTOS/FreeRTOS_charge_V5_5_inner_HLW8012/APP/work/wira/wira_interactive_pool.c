#include "work.h"
#define PRT_LOG(X...)	//sysprt->alog(X)


#define Max_Pool	20		//����������

#define no_using	0
#define is_using	1

pool_pt pool_head;
static uint32_t num_cntr = 0;

//��ʼ�� ������
static void init( void )
{
	int i=0;
	pool_head = NULL;
	
}

//ע�� һ�� ������ �� ������
static void register_one( pool_pt pool )
{
	pool_pt tpool;
	if( pool == NULL )
	{
		PRT_LOG( "error:register_one	\r\n " );
		return ;
	}			

	if( pool_head == NULL )
	{
		PRT_LOG( "register_one:	pool_head == NULL	\r\n " );
		PRT_LOG( "  pool->num =	%d \r\n ",pool->num		);
		
		pool_head = pool;
		pool_head->last = NULL;
		pool_head->next = NULL;
	}
	else
	{
		PRT_LOG( "register_one:	pool_head != NULL	\r\n " );
		PRT_LOG( "  pool->num =	%d \r\n ",pool->num		);
		//��ͷ��ʼ��β��
		tpool = pool_head;
		while( tpool->next != NULL )
		{
			tpool = tpool->next;
		}
		//���pool ��β��
		tpool->next = pool;
		
		//tpool��Ϊpool����һ��
		pool->last = tpool;
		
		//pool��Ϊ�µ�β��
		pool->next = NULL;
		
	}
}

//�� ������ ɾ�� һ�� ������
static int delate_one( pool_pt pool )
{
	pool_pt lpool,npool;		//�ֱ�������ȡ������һ���һ��
	
	if( pool->last == NULL )
	{//˵�� ����������ͷ
		
		PRT_LOG( " pool->last == NULL \r\n " );
		if( pool->num != pool_head->num )
		{//˵����������ͷ��Ϊ�Ƿ�����(��ͷ��һ��Ҳ�Ƿ��������num��Ψһ��)
			//����Ҫ���κβ���
			PRT_LOG( "pool->num = %d \r\n ",pool->num );
			PRT_LOG( "pool_head->num = %d \r\n ",pool_head->num );
			
			
			PRT_LOG( "error: pool->last == NULL \r\n " );
			return -1;
		}
		
		if( pool->next == NULL )
		{//˵��Ҳ������β��( ����ͷ��Ҳ��β )��������ֻ����1��	
			PRT_LOG( "pool->next == NULL \r\n " );
			pool_head = NULL;	//ֱ�Ӱ�pool_head����Ϊ NULL��˵��û�� ������ 
			
		}
		else
		{//˵���Ǳ�ͷ������β
			PRT_LOG( "it is head \r\n " );
			pool_head = pool_head->next;		//��ͷָ�뻬 ����һ��
			pool_head->last = NULL;					//��ɱ�ͷ
		}
	}
	else if( pool->next == NULL )
	{//˵������������β��
		PRT_LOG( "pool->next == NULL \r\n " );
		lpool = pool->last;	//��ȡ����β�͵� ��һ�� 
		lpool->next = NULL;	//����һ�� ����Ϊ����β��
	}
	else
	{//˵�����м��� m
		PRT_LOG( "other \r\n " );
		lpool = pool->last;
		npool = pool->next;
		
		//����pool->next �� lpool->next
		lpool->next = pool->next;
		//����lpool�� npool->last		
		npool->last = lpool;
		
	}
	return 1;
}

//����һ��������
static pool_pt create( uint32_t dest,uint32_t wait,event_pt event )
{
	
	uint8_t *src = NULL;
	uint8_t *dst = NULL;
	
	//�����ڴ�
	pool_pt pool = mem_alloc( sizeof( pool_t ) );
	
	if( pool == NULL ) return NULL;
	
	/*------------------------------------*/
	//��ʼ��������
	pool->last = NULL;
	
	pool->sta = 0;
	pool->num = num_cntr++;
	pool->dest_cntr = dest;
	pool->now_cntr = 0;
	pool->wait_tick = wait;
	pool->otick = get_sys_ticks();
	
	/*------------------------------------*/
	//����event
	src = (uint8_t*)event;
	dst = (uint8_t*)( &( pool->event ) );
	
	for( int i=0;i<sizeof( event_t ) ;i++ )
	{
		dst[i] = src[i];
	}
	/*------------------------------------*/
	
	pool->next = NULL;
	PRT_LOG( "c pool->num = %d \r\n",pool->num );
	PRT_LOG( "c pool->event.ddh = %s \r\n",pool->event.ddh );
	
	
	
	return pool;
	
}

//����
static void destroy( pool_pt pool )
{
	/*------------------------------------*/
	//�ͷ��ڴ�
	mem_free( pool );
	
}

//���ݶ������ҵ� ������
static pool_pt get_by_ddh( char* ddh )
{
	int i=0;
	pool_pt pool = pool_head;		//���ڱ�ʾ���ڴ���Ķ���head -> ....
	while( pool != NULL )
	{
		//-----------------------------------
		//�Ƚ�ddh�Ƿ�һ��
		for( i=0;i<MaxDDH_Len;i++ )
		{
			if( ddh[i] != pool->event.ddh[i] ) break;
		}
		
		if( i == MaxDDH_Len )
		{//˵��ddhһ��
			PRT_LOG( "find success:get_by_ddh->num = %d \r\n",pool->num );
			return pool;
		}
		
		//-----------------------------------
		if( pool->next != NULL ) 
			pool = pool->next;				//�л�����
		else 
			break;										//����ѭ��
	}
	PRT_LOG( "find faild:get_by_ddh \r\n" );
	return NULL;

}

static void deal_delate( void )
{
	pool_pt pool = pool_head;		//���ڱ�ʾ���ڴ���Ķ���head -> ....
	
	while( pool != NULL )
	{
		//������Ҫɾ������
		if( pool->sta == 2 )
		{
			//�ӽ�������ɾ����һ��
			ipool->delate_one( pool );
			ipool->destroy( pool );
			break;
		}
		//-----------------------------------
		if( pool->next != NULL ) 
			pool = pool->next;				//�л�����
		else 
			break;										//����ѭ��
		
	}
	
}

static void work( void )
{
	pool_pt pool = pool_head;		//���ڱ�ʾ���ڴ���Ķ���head -> ....
	time_t ntick = 0;
	//������������״̬������
	while( pool != NULL )
	{
		//-----------------------------------
		switch( pool->sta )
		{
			case 0://�����������������������������
				//��������
				switch( pool->event.type )
				{
					//�����ϱ�
					case _event_curr:	break;
						
					//�����¼�
					case _event_sock:	event_report_sock( &(pool->event) ); break;
						
					//�����¼�
					case _event_net:	event_report_net( &(pool->event) ); break;
					
					default:
						break;
				}
				
				//ͳ�Ʒ��ʹ���
				if( pool->dest_cntr > pool->now_cntr )
				{
					pool->now_cntr++;
					pool->sta = 1;			//�ȴ�һ��ʱ��
				}
				else
				{
					pool->sta = 2;			//ȥɾ����Ԫ��
				}
			
				break;
				
			case 1://�ȴ� ���� �δ�
				ntick = get_sys_ticks();
				if( pool->wait_tick > ( ntick - pool->otick ) ) break;
			
				pool->otick = ntick;
							
				pool->sta = 0;
				
				break;
			
			case 2://�ȴ�ɾ�� �������
				
				break;
			
			default:
				break;
		}
		
		
		//-----------------------------------
		if( pool->next != NULL ) 
			pool = pool->next;				//�л�����
		else 
			break;										//����ѭ��
		
	}

	deal_delate();
	
}




static ipool_t m_ipool = {
	init,
	register_one,
	delate_one,
	create,
	destroy,
	get_by_ddh,
	work
};

ipool_pt ipool = &m_ipool;

