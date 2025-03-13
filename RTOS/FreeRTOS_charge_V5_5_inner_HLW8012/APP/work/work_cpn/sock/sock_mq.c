
#include "work.h"

static thread_mq_t event_mq;
static thread_mq_t order_mq;
static int mq_init_flag = 0;

static void init( void )
{
	
	if( 1 == mq_init_flag ) return;
	//����sock���ⷢ���¼�
	event_mq = thread_mq_init("event_mq", sizeof(event_t), 20);
	//������sock��������
	order_mq = thread_mq_init("order_mq", sizeof(order_t), 20);
	mq_init_flag = 1;
}


static void copy_ddh_to_order( char* ddh,order_pt order )
{
	int i = 0;
	for( i=0;i<MaxDDH_Len;i++ ) order->ddh[i] = 0;
	i = 0;
	while( ddh[i] != '\0' )
	{
		order->ddh[i] = ddh[i];
		i++;
		if( i>=MaxDDH_Len ) break;
	}
	order->ddh[MaxDDH_Len-1] = '\0';
}
static void copy_ddh_to_event( char* ddh,event_pt event )
{
	int i = 0;
	for( i=0;i<MaxDDH_Len;i++ ) event->ddh[i] = 0;
	i = 0;

	while( ddh[i] != '\0' )
	{
		event->ddh[i] = ddh[i];
		i++;
		if( i>=MaxDDH_Len ) break;
	}
	event->ddh[MaxDDH_Len-1] = '\0';
}

//���������������Ϳ�������
/*------------------------------------------------------------------------*/

static void send_order( char* ddh,int num, int cmd, uint32_t dat,uint32_t param  )
{
	int ret = 0;
	order_t order;
	copy_ddh_to_order( ddh,&order );
	order.num = num;
	order.cmd = cmd;
	order.dat = dat;
	order.param = param;
	
	if( mq_init_flag == 0 ) return;
	ret = thread_mq_send( order_mq,&order, sizeof(order_t), 0x01 );
	if( ret == bFALSE )
	{
		sysprt->aerr("send_order \r\n");
	}
}

//�����������걨�¼�
static void put_event( char* ddh,int type,int num, int val, uint32_t dat,double curr,void* period_e )
{
	int ret;
	event_t event;
	copy_ddh_to_event( ddh,&event );
	event.type = type;			//�¼���		
	event.num = num;				//�¼���
	event.val = val;				//�¼�����ֵ,û����Ϊ0
	event.dat = dat;				//�¼��漰��������
	event.curr = curr;			//�¼��漰���ĸ�����
	if( period_e != PNULL )
	{
		for( int i=0;i<MaxPeriodE;i++ )
			event.period_e[i] =  ((period_e_t*)period_e)[i];
	}
	
	if( mq_init_flag == 0 ) return;
	ret = thread_mq_send( event_mq,&event, sizeof(event_t), 0x01 );
	if( ret == bFALSE )
	{
		sysprt->aerr("put_event \r\n");
	}
}
/*------------------------------------------------------------------------*/

static int recv_order( order_pt order )
{
	if( mq_init_flag == 0 ) return -1;
	int ret = thread_mq_recv( order_mq, order, sizeof(order_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
	
}

static int recv_event( event_pt event )
{
	if( mq_init_flag == 0 ) return -1;
	int ret = thread_mq_recv( event_mq, event, sizeof(event_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
	
}

/*------------------------------------------------------------------------*/
static sock_mq_t do_sock_mq ={
	init,
	send_order,
	put_event,
	recv_order,
	recv_event
};

sock_mq_pt sock_mq = &do_sock_mq;
