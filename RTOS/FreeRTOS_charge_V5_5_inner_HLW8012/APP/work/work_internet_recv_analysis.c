
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)//dprintf(X)	

/*--------------------------------------------------------------------------------*/
//服务器命令处理

extern int rp_serv_first_boot;		//描述是否需要告诉服务器第一次启动

static void report_serv_update_success(void)
{
	char *ddh = NULL;
	time_t tick = 0;
	PRT_LOG("======>report_serv_update_success\r\n");
	ddh = mem_alloc( MaxDDH_Len );
	
			
	/*--------------------------------------------------------------------------------*/
	tick = get_sys_ticks();
	//查看是否需要通知服务器本次开机成功
	if( rp_serv_first_boot )
	{
		//按照(事件号+i+tick)%8的方式生成订单号，让订单号看起来是随机的
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
	//检查插座的情况，如果是关的，则请求，发送消息给服务器，用于结束异常断电导致的订单。
	//TODO_START:

	tick = get_sys_ticks();

	for( int id=0;id<MaxSock;id++ )
	{
		//正在充电就不用管
		if( cat_sock_working( id ) ) continue;
		
		//插座如果不是在工作中，则需要上报一下
		PRT_LOG( "report_serv_net_reconnect:sock[%d] is stop,report to serv\r\n",id+1 );
		
		//如果分配失败，则此功能就不用了
		if( ddh == NULL ) break; 
		
		//按照(id+i+tick)%8的方式生成订单号，让订单号看起来是随机的
		ddh[3]= Brd_Version+'0';
		ddh[2]= Brd_SubVersion+'0';
		ddh[1]= App_Version+'0';
		ddh[0]= App_SubVersion+'0';
		
		for( int i=4;i<20;i++ )	ddh[i] = ( (id+i+tick)&7 )+'0';	
		ddh[16] = 0; ddh[17] = '\0';
		
		//发送事件,根据id知道是哪个插座
		sock_mq->put_event( ddh,_event_net,id,0,0,0,PNULL );

		
		
	}
		//TODO_END	

	mem_free( ddh );
}

static uint8_t rp_net_sta = 0;
static time_t otick_rp = 0;
static time_t ntick_rp = 0;


//报告网络重连的标志复位:成功联网后，需要复位状态
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
			
			
			//联网20秒钟后，检查插座情况，并告诉服务器
			report_serv_net_reconnect();
			
			rp_net_sta++;
			PRT_LOG("======>rp_work: -->%d,[%010u]\r\n",rp_net_sta,ntick_rp );
			break;
		
		case 2://此模块工作已经完成
			break;
		
		default:
			break;
	}
}
/*--------------------------------------------------------------------------------*/
//获取分时时段
static void get_slot( void )
{
	static uint32_t otick_slot = 0;
	uint32_t ntick =0;

	if( guide_dat->time_slot->get_slot != 0 ) return;
	
	ntick = get_sys_ticks();
	if( ( ntick - otick_slot ) <= 5000 ) return;
	otick_slot = ntick;
	
	//定时发送
	air_man->send_str( air724, "GETJFPG\r\n" );	
	
	show_send_data( "GETJFPG\r\n" );
}

/*--------------------------------------------------------------------------------*/
//定时同步网络时间
static void get_net_time( void )
{
	static uint32_t otick_slot = 0;
	uint32_t ntick =0;
	
	ntick = get_sys_ticks();
	if( ( ntick - otick_slot ) <= (10*60*1000) ) return;
	otick_slot = ntick;
	
	//定时发送
	air_man->send_str( air724, "GETTIME\r\n" );	
	
	show_send_data( "GETTIME\r\n" );
}


/*--------------------------------------------------------------------------------*/

//轮询作业，接收并解析数据
void work_internet_recv_analysis( wk_inet_pt inet )
{
//	char *ddh = NULL;
//	time_t tick = 0;
	switch( inet->ssta )
	{
		//
		case 0://初始化工作数据
			PRT_LOG("work_internet_recv_analysis \r\n");
			//接收缓冲区复位
			wira_work_data_rst();
			
			//如果是第一次启动，则通知服务器，更新成功
			report_serv_update_success();
			
			//复位网络重连报告状态机,稍后十秒会检查插座的状态。
			//如果插座是关的，则告诉服务器
//			rp_net_sta_rst();
			
			nwdt->feed( inet ); //喂狗
			inet->ssta++;
			
			break;
		
		case 1://与服务器交流
		
			//超时机制处理
			if ( nwdt->deal( inet )<0 ) break;

			//获取分时时段
			get_slot();

			//定时同步网络时间
			get_net_time();
		
			//联网成功后要通知服务器
//			rp_work();
		
			//接收数据并解析
			wira_wrok_cycle( inet );
		
			//硬件事件上报 herp
			wira_event_report();
			
			//交互事件处理
			ipool->work();
			
		
			break;
		
		default:
			break;
	}
	

}





