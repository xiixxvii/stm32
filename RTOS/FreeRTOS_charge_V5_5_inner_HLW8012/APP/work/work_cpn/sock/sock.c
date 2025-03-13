

#include "work.h"

extern sock_hlw_opt_pt sock_hlw_opt;


extern void sock_ctrl_work( void );


sock_sta_t socks[MaxSock] ={
	{0,0},
	{0,0},
};

void sock_init(void)
{
	//����
	sock_mq->init();
	sock_hlw_opt->init();
	led_board->set_fuse_led( __ON );
	
	led_board->mode_req( 0,_e_mode_standing,NULL,NULL );
	led_board->mode_req( 1,_e_mode_standing,NULL,NULL );
}


//����n���,���û�ж������У��������豸
void rst_dev_over_week(void)
{
	int ret = 0;
	//��鿪����û�дﵽ3��
	if( get_sys_ticks() < (3*24*3600*1000)) return;
		
	//��ȡ��������״̬
	ret = 0;
	for( int id=0;id<MaxSock;id++ )
	{
		if( cat_sock_working( id ) ) ret |=1;
		ret = ret<<1;
	}
	
	//���û�в������ڹ������������豸
	if( ret == 0 )
		reset_system();
	
}




void sock_work_entry( void )
{
	//��ʱ����һ���豸(���û�в������ڹ���)
	rst_dev_over_week();
	
	//��������˿״̬
	listen_fuse();

	//��������״̬
//	sock_hlw_opt->work();
	
	//�������ƹ���
	sock_ctrl_work();
	
	//�ȴ����綩��ָ��
	wait_net_order();
	
}
