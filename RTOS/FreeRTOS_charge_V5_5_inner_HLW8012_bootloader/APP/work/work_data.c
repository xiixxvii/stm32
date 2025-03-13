#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

/*----------------------------------------------------------------------------------*/

static void read_fac_conf_data( wdat_pt wdat )
{

	uint32_t *dst = (uint32_t*)wdat; 
	uint32_t len = sizeof( wdat_t )/4;
//	sysprt->alog( "size = %d \r\n",sizeof( wdat_t ) );

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
//	PRT_LOG("erase_fac_conf_data\r\n");
	
	FLASH_Erase( addr_config );
	
	
}
static void write_fac_addr_conf_data( void )
{
	uint32_t len = sizeof( wdat_t )/4;
	FLASH_Write( addr_config,(uint32_t*)(wdat), len );
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


void work_data_load( void )
{
			
	read_fac_conf_data( wdat );
	if( wdat->boot_code != BOOT_CODE )
	{
		sysprt->alog( "It's first boot \r\n" );
		//初始化数据
		wdat->boot_code = BOOT_CODE;
//		wdat->program = _app_update;
		wdat->program = _app_new;					//批量的时候，直接启动app
		wdat->net_mode = NetMode_Air780;
		wdat->zd_val = 0;									//批量的时候，终端默认是0
		wdat->curr_ratio[0] = 1.001;
		wdat->curr_ratio[1] = 1.001;
		
		
		wdat->k[0] = 5.2;
		wdat->k[1] = 5.34;
		
		wdat->b[0] = 0;
		wdat->b[1] = 0;


		wdat->ota_server = _ota_ydc_serv;		//默认华志的服务器
		work_data_write();
	}
	else
		sysprt->alog( "It's not first boot \r\n" );
	
	wdat->net_ptf = 1;
}
/*----------------------------------------------------------------------------------*/








