#include "work.h"

#define PRT_LOG(X...)	sysprt->alog(X)	

static void save_data(void)
{
	PRT_LOG("save_data..." );
	work_data_write();
	PRT_LOG("===>OK\r\n" );
}
/*-------------------------------------------------------------------------*/
#define check_argc_error( ival,jump ) \
	do{ \
		if( argc <ival ){ \
			PRT_LOG( "err: argc < %d \r\n",ival ); \
			goto jump; \
		} \
	}while(0);

static int task_config( char argc, char **argv )
{
	uint32_t val = 0;
//	double ratio = 0; 
	double out_ratio = 0,now_ratio = 0; 
	float k=0,b=0;
	
	check_argc_error( 2,__usage_cmd );
	
	if( 0 == strcmp( argv[1],"zd_val" ) )
	{//配置终端号
		check_argc_error( 3,__usage_zd_val );
		goto __deal_zd_val;
	}
	else if( 0 == strcmp( argv[1],"ratio" ) )
	{//电流系数
		check_argc_error( 5,__usage_ratio );
		goto __deal_ratio;
	}
	else if( 0 == strcmp( argv[1],"rst_ratio" ) )
	{//复位电流系数 
		check_argc_error( 2,__usage_rst_ratio );
		goto __deal_rst_ratio;
	}
	else if( 0 == strcmp( argv[1],"kb" ) )
	{//直接配置,拟合直线系数 k和b
		check_argc_error( 4,__usage_kb );
		goto __deal_kb;
	}
	else if( 0 == strcmp( argv[1],"rst_kb" ) )
	{//直接配置,拟合直线系数 k和b
		check_argc_error( 2,__usage_rst_kb );
		goto __deal_rst_kb;
	}
	else if( 0 == strcmp( argv[1],"store" ) )
	{//存储配置
		check_argc_error( 2,__usage_store );
		goto __deal_store;
	}
	else if( 0 == strcmp( argv[1],"net_ptf" ) )
	{//配置 网络打印
		check_argc_error( 3,__usage_net_ptf );
		goto __deal_net_ptf;
	}
	else if( 0 == strcmp( argv[1],"default" ) )
	{////配置数据复位
		check_argc_error( 2,__usage_default );
		goto __deal_default;
	}
	else if( 0 == strcmp( argv[1],"net" ) )
	{////配置上网方式
		check_argc_error( 3,__usage_net );
		goto __deal_net;
	}
	else if( 0 == strcmp( argv[1],"ota_server" ) )
	{////配置上网方式
		check_argc_error( 3,__usage_ota_server );
		goto __deal_ota_server;
	}	
	return 0;
//------------------------------------------------------------------	
__usage_cmd:
	PRT_LOG("->config <cmd>\r\n");
	PRT_LOG("cmd:zd_val | ratio | hlw_ptf | net_ptf | store | default | net\r\n" );
	return -1;
//------------------------------------------------------------------	
__usage_zd_val:
	
	return -2;
__deal_zd_val:
	wdat->zd_val = atol( argv[2] );	
	work_data_write();
	PRT_LOG("config zd_val -->%d \r\n",wdat->zd_val );
	return 0;
//------------------------------------------------------------------	
__usage_ratio:
	PRT_LOG("->config ratio <sock> <out> <now>\r\n");
	PRT_LOG("\t<sock>: 0 / 1 \r\n");
	PRT_LOG("\t<out> : out curr val,eg. 2.30 \r\n");		//电流表读到的数据
	PRT_LOG("\t<now> : check curr val,eg. 1.30 \r\n");	//串口输出值，可能不准
	
	return -2;
	
__deal_ratio:
	val = atol( argv[2] );
	if( val >= 2 )
	{
		PRT_LOG( "err: sock only support: 0~1\r\n" );
		goto __usage_ratio;
		
	}
	out_ratio = atof( argv[3] );
	now_ratio = atof( argv[4] );

	
//	wdat->curr_ratio[ val ] = now_ratio/wdat->curr_ratio[ val ]/out_ratio;
	wdat->curr_ratio[ val ] = now_ratio/out_ratio;
	PRT_LOG("config ratio %d -->%f \r\n",val,wdat->curr_ratio[ val ] );
	save_data();
	return 0;
//------------------------------------------------------------------	
//------------------------------------------------------------------	
__usage_rst_ratio:
	PRT_LOG("->config rst_ratio\r\n");
	return -2;
	
__deal_rst_ratio:
	val = atol( argv[2] );	//插座号
	if( val >= 2 )
	{
		PRT_LOG( "err: sock only support: 0~1\r\n" );
		goto __usage_ratio;
		
	}
	wdat->curr_ratio[ val ] = 1.0;
	
	PRT_LOG("config rst_ratio [%d]-->1.0 \r\n",val );
	return 0;
//------------------------------------------------------------------	
__usage_kb:
	
	PRT_LOG("->config kb <sock> <kval> <bval>\r\n");
	PRT_LOG("\t<sock>: 0 / 1 \r\n");
	PRT_LOG("\t<val> : y = kx + b \r\n");
	
	return -2;
	
__deal_kb:
	val = atol( argv[2] );	//插座号
	if( val >= 2 )
	{
		PRT_LOG( "err: sock only support: 0~1\r\n" );
		goto __usage_kb;
		
	}
	k = atof( argv[3] );
	b = atof( argv[4] );
	
	
//	wdat->curr_ratio[ val ] = now_ratio/wdat->curr_ratio[ val ]/out_ratio;
	wdat->k[ val ] = k;
	wdat->b[ val ] = b;
	PRT_LOG("config kb[%d] -->y = %.5f + %.5f \r\n",val,wdat->k[ val ],wdat->b[ val ] );
	save_data();

	return 0;
//------------------------------------------------------------------	
__usage_rst_kb:
	PRT_LOG("->config rst_kb <sock>\r\n");
	PRT_LOG("\t<sock>: 0 / 1 \r\n");
	return -2;
	
__deal_rst_kb:
	val = atol( argv[2] );	//插座号
	if( val >= 2 )
	{
		PRT_LOG( "err: sock only support: 0~1\r\n" );
		goto __usage_rst_kb;
		
	}
	wdat->k[ val ] = 1;
	wdat->b[ val ] = 0;
	PRT_LOG("config rst_kb [%d] -->y = %.5f + %.5f  \r\n",val,wdat->k[ val ],wdat->b[ val ] );
	return 0;

//------------------------------------------------------------------	
		
__usage_store:
	
	return -2;
__deal_store:
	work_data_write();
	PRT_LOG("config store -->OK \r\n" );
	
	guide_dat->order_info->sock[0].working = 1;
	guide_dat->order_info->sock[0].end.Year = 2024;
	guide_dat->order_info->sock[0].end.Month = 11;
	guide_dat->order_info->sock[0].end.Date = 27;
	guide_dat->order_info->sock[0].end.Hour = 2;
	guide_dat->order_info->sock[0].end.Minute = 10;
	guide_dat->order_info->sock[0].end.Second = 2;
	
	wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
	
	
	return 0;
//------------------------------------------------------------------	

//------------------------------------------------------------------	

__usage_net_ptf:
	
	return -2;
__deal_net_ptf:
	val = atol( argv[2] );
	wdat->net_ptf = val;
	PRT_LOG("config net_ptf -->%d \r\n",val );
	return 0;
	
//------------------------------------------------------------------	
__usage_default:
	
	return -2;
__deal_default:
	wdat->boot_code = 0;
	wdat->app_dat.init_code = 0;

	PRT_LOG("config resrt -->OK \r\n",val );
	save_data();
	reset_system();
	while(1);
	return 0;
//------------------------------------------------------------------	
__usage_net:
	
	return -2;
__deal_net:
	if( 0 == strcmp( argv[2],"air724" ) )
	{
		wdat->net_mode = NetMode_Air724;
	}
	else if( 0 == strcmp( argv[2],"air780" ) )
	{
		wdat->net_mode = NetMode_Air780;
	}
	else
	{
		wdat->net_mode = NetMode_Not;
	}
	PRT_LOG("config net %s-->OK \r\n",argv[2],val );
	save_data();
	return 0;

//------------------------------------------------------------------	
__usage_ota_server:
	PRT_LOG("->config ota_server <serv>\r\n");
	PRT_LOG("\t<serv>: ota_server/test_server \r\n");
	return -2;
__deal_ota_server:
	if( 0 == strcmp( argv[2],"test_server" ) )
	{
		wdat->ota_server = _ota_test_serv;
	}
	else
	{
		wdat->ota_server = _ota_ydc_serv;
	}
	PRT_LOG("config ota_server %s-->OK \r\n",argv[2] );
	save_data();
	return 0;

//------------------------------------------------------------------	
	
}
cmd_util_t __cmd_config={
	.name	 	= "config",
	.usage 	= "config",
	.text 	= "->input config to cat usage",
	.app 		= task_config,
};

/*-------------------------------------------------------------------------*/

void prt_date_time( rtc_dt_t* dt,char* str )
{
	PRT_LOG("%s:%04d/%02d/%02d-%02d:%02d:%02d\r\n",str,dt->Year,dt->Month,dt->Date,   dt->Hour,dt->Minute,dt->Second);
		
}


static int task_cat_data( char argc, char **argv )
{
//	uint32_t val = 0;
//	double ratio = 0; 
	if( argc >2 ) return -1;

	
	PRT_LOG("boot_code -->%d \r\n",wdat->boot_code );
	PRT_LOG("program -->%d \r\n",wdat->program );
	PRT_LOG("net_mode -->%d \r\n",wdat->net_mode );
	PRT_LOG("net_ptf -->%d \r\n",wdat->net_ptf );
	PRT_LOG("zd_val -->%d \r\n",wdat->zd_val );

	PRT_LOG("curr_ratio 0 -->%f \r\n",wdat->curr_ratio[0] );
	PRT_LOG("curr_ratio 1 -->%f \r\n",wdat->curr_ratio[1] );

	PRT_LOG("y0 = %f *x0 + (%f) \r\n",wdat->k[0],wdat->b[0] );
	PRT_LOG("y1 = %f *x1 + (%f) \r\n",wdat->k[1],wdat->b[1] );
	
	
	PRT_LOG("ota_server -->%d \r\n",wdat->ota_server );
	switch( wdat->ota_server )
	{
		case 0: PRT_LOG("\t _ota_ydc_serv\r\n"); break;
		case 1: PRT_LOG("\t _ota_test_serv\r\n"); break;
		default: PRT_LOG("\t ota_config_error\r\n"); break;
	}

	rtc_dt_t *dt = 0;
	//打印订单情况
	for( int id=0;id<MaxSock;id++ )
	{
		PRT_LOG("\r\nSock[%d]\r\n",id );
		
		
		PRT_LOG("working:%d\r\n",guide_dat->order_info->sock[id].working );
		PRT_LOG("ddh    :%s\r\n",guide_dat->order_info->sock[id].ddh );
		
		dt = &(guide_dat->order_info->sock[id].start);
		prt_date_time(dt,"start  ");
		dt = &(guide_dat->order_info->sock[id].end);
		prt_date_time(dt,"end    ");
		
		PRT_LOG("\r\n\r\n");

		
	}
	
	
	return 0;
}
cmd_util_t __cmd_cat_data={
	.name	 	= "cat_data",
	.usage 	= "cat_data",
	.text 	= "->cat_data zd_val | curr_ratio0 | curr_ratio1 ",
	.app 		= task_cat_data,
};
/*-------------------------------------------------------------------------*/
static int task_ctrl( char argc, char **argv )
{
	uint32_t sock = 0;
//	uint32_t pwr = 0;
	time_t time = 0;
	int cmd = 0;
	
	char *ddh="0000000000000000";
	check_argc_error( 4,__usage_cmd );
	
	sock = atol( argv[1] );	//0 1 
	
	if( 0 == strcmp( argv[2],"open" ) )
	{	
//		pwr = __ON;
		time = atol( argv[3] )*1000*60;
		cmd = oct_start_charge;
	}
	else
	{	
//		pwr = __OFF;
		time = 0;
		cmd = oct_cancel_charge;
	}
	
	//产生充电订单
	sock_mq->send_order( ddh, sock, cmd, time,0 );
	
	
	PRT_LOG("ctrl ->sock[%d] %s ->%d min \r\n",sock,argv[2],time );
	return 0;

__usage_cmd:
	PRT_LOG("ctrl ->sock[%d] %s ->%d min \r\n",sock,argv[2],time );
	return -1;
	
}
cmd_util_t __cmd_ctrl={
	.name	 	= "ctrl",
	.usage 	= "ctrl <sock> <open/close> <time>",
	.text 	= "-- ",
	.app 		= task_ctrl,
};

/*-------------------------------------------------------------------------*/
static int task_ota( char argc, char **argv )
{
	PRT_LOG("user req ota\r\n" );

	wdat->program = _app_update;
	req_goto_update();
	
	return 0;
}
cmd_util_t __cmd_ota={
	.name	 	= "ota",
	.usage 	= "ota",
	.text 	= "->ota app1 ",
	.app 		= task_ota,
};
/*-------------------------------------------------------------------------*/
static int task_set_boot( char argc, char **argv )
{
	PRT_LOG("\terr:please go to boot loader set boot\r\n");
	
	
	return 0;
}
cmd_util_t __cmd_set_boot={
	.name	 	= "set_boot",
	.usage 	= "set_boot <app>",
	.text 	= "->set_boot <ota/app1> ",
	.app 		= task_set_boot,
};
/*-------------------------------------------------------------------------*/

//注册扩展调试命令
void register_debug_cmd(void)
{
	cmdline_man->regiser_one( &__cmd_config );
	cmdline_man->regiser_one( &__cmd_cat_data );
	cmdline_man->regiser_one( &__cmd_ctrl );
	cmdline_man->regiser_one( &__cmd_ota );
	cmdline_man->regiser_one( &__cmd_set_boot );
	
}
