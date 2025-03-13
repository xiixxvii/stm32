

#include "work.h"

extern sock_hlw_opt_pt sock_hlw_opt;


extern void sock_ctrl_work( void );


sock_sta_t socks[MaxSock] ={
	{0,0},
	{0,0},
};

void sock_init(void)
{
	//设置
	sock_mq->init();
	sock_hlw_opt->init();
	led_board->set_fuse_led( __ON );
	
	led_board->mode_req( 0,_e_mode_standing,NULL,NULL );
	led_board->mode_req( 1,_e_mode_standing,NULL,NULL );
}


//开机n天后,如果没有订单进行，则重启设备
void rst_dev_over_week(void)
{
	int ret = 0;
	//检查开机有没有达到3天
	if( get_sys_ticks() < (3*24*3600*1000)) return;
		
	//获取插座工作状态
	ret = 0;
	for( int id=0;id<MaxSock;id++ )
	{
		if( cat_sock_working( id ) ) ret |=1;
		ret = ret<<1;
	}
	
	//如果没有插座正在工作，则重启设备
	if( ret == 0 )
		reset_system();
	
}




void sock_work_entry( void )
{
	//定时重启一遍设备(如果没有插座正在工作)
	rst_dev_over_week();
	
	//监听保险丝状态
	listen_fuse();

	//监听电流状态
//	sock_hlw_opt->work();
	
	//插座控制工作
	sock_ctrl_work();
	
	//等待网络订单指令
	wait_net_order();
	
}
