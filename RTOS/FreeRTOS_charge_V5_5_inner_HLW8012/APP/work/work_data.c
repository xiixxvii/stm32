#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

/*----------------------------------------------------------------------------------*/

static void read_fac_conf_data( wdat_pt wdat )
{

	uint32_t *dst = (uint32_t*)wdat; 
	uint32_t len = sizeof( wdat_t )/4;
	sysprt->alog( "size = %d \r\n",sizeof( wdat_t ) );

	for( int i=0;i<len;i++   )
	{
		*dst =  *((volatile uint32_t *)(addr_config+i*4));
		dst++;
	}
//	eep_24c0x->read( eep_dev_addr0,0,(uint8_t *)(wdat),sizeof( wdat_t ) );	
}


static void erase_fac_conf_data( void )
{
	uint32_t waddr = addr_config;
	PRT_LOG("erase_fac_conf_data\r\n");
	
	FLASH_Erase( addr_config );
	
	
}
static void write_fac_addr_conf_data( void )
{
	uint32_t len = sizeof( wdat_t );
	FLASH_Write( addr_config,(uint32_t*)(wdat), len/4 );
}
void work_data_write( void )
{
	erase_fac_conf_data();
	write_fac_addr_conf_data();
//	eep_24c0x->write( eep_dev_addr0,0,(uint8_t *)(wdat),sizeof( wdat_t ) );
}
/*----------------------------------------------------------------------------------*/


static wdat_t m_wdat;

wdat_pt wdat = &m_wdat;

int rp_serv_first_boot = 0;

void work_data_load( void )
{
	int save = 0;

	read_fac_conf_data(wdat);
//	eep_24c0x->read( eep_dev_addr0,0,(uint8_t *)(wdat),sizeof( wdat_t ) );
	if( wdat->boot_code != BOOT_CODE )
	{
		sysprt->alog( "It's first boot \r\n" );
		//初始化数据
		wdat->boot_code = BOOT_CODE;
		wdat->program = 0;
		wdat->net_mode = NetMode_Air724;
		wdat->rp_boot = 0;
		wdat->zd_val = 1;

		wdat->curr_ratio[0] = 1.001;
		wdat->curr_ratio[1] = 1.001;
		
		
		wdat->k[0] = 5.2;
		wdat->k[1] = 5.34;
		
		wdat->b[0] = 0;
		wdat->b[1] = 0;


		wdat->ota_server = _ota_ydc_serv;
		
		save ++;
		
		
		
	}
	else
	{
		sysprt->alog( "It's not first boot \r\n" );
		
		//判断不合法的值
		for( int id=0;id<MaxSock;id++ )
		{
//			if( (wdat->k[id] == 0xFF) || (wdat->k[id] == 0) )	
//			{//说明是旧版那几个样机
//				sysprt->alog( "err: wdat->k[%d]\r\n",id );
//				wdat->k[id] = 0.88;
//				wdat->b[id] = -0.0018;
//				
//				work_data_write();
//			}
				
		}
		
	/*-------------------------------------------------------*/
		if( wdat->rp_boot )
		{//此标识用于向服务器描述第一次启动(更新完APP)
			sysprt->alog( "rp_serv_first_boot = 1 \r\n" );
			rp_serv_first_boot = 1;
			wdat->rp_boot = 0;
			save ++;
		}
	}
	
	/*-------------------------------------------------------*/
	//处理app数据
	if( wdat->app_dat.init_code != BOOT_CODE )
	{
		sysprt->alog( "app_dat is first init \r\n" );
		wdat->app_dat.init_code = BOOT_CODE;
		wdat->app_dat.tick_put_curr = 3*60*1000;	//默认3分钟传输一次电流
	}
	else
	{
		sysprt->alog( "app_dat is not first init \r\n" );
		
		//检查定时传电流的值是否异常，如果异常，则设置为3分钟传一次
		if( ( wdat->app_dat.tick_put_curr == 0 )||( wdat->app_dat.tick_put_curr>10*60*1000 ) )
		{//说明异常
			wdat->app_dat.tick_put_curr = 3*60*1000;	//默认3分钟传输一次电流
			save++;
		}
		
	}
	
	/*-------------------------------------------------------*/
	//如果本次需要存储，则写数据到eeprom
	if( save != 0 )
	{
		work_data_write();
	}
	
	wdat->net_ptf = 1;
}





