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
		//��ʼ������
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
		
		//�жϲ��Ϸ���ֵ
		for( int id=0;id<MaxSock;id++ )
		{
//			if( (wdat->k[id] == 0xFF) || (wdat->k[id] == 0) )	
//			{//˵���Ǿɰ��Ǽ�������
//				sysprt->alog( "err: wdat->k[%d]\r\n",id );
//				wdat->k[id] = 0.88;
//				wdat->b[id] = -0.0018;
//				
//				work_data_write();
//			}
				
		}
		
	/*-------------------------------------------------------*/
		if( wdat->rp_boot )
		{//�˱�ʶ�����������������һ������(������APP)
			sysprt->alog( "rp_serv_first_boot = 1 \r\n" );
			rp_serv_first_boot = 1;
			wdat->rp_boot = 0;
			save ++;
		}
	}
	
	/*-------------------------------------------------------*/
	//����app����
	if( wdat->app_dat.init_code != BOOT_CODE )
	{
		sysprt->alog( "app_dat is first init \r\n" );
		wdat->app_dat.init_code = BOOT_CODE;
		wdat->app_dat.tick_put_curr = 3*60*1000;	//Ĭ��3���Ӵ���һ�ε���
	}
	else
	{
		sysprt->alog( "app_dat is not first init \r\n" );
		
		//��鶨ʱ��������ֵ�Ƿ��쳣������쳣��������Ϊ3���Ӵ�һ��
		if( ( wdat->app_dat.tick_put_curr == 0 )||( wdat->app_dat.tick_put_curr>10*60*1000 ) )
		{//˵���쳣
			wdat->app_dat.tick_put_curr = 3*60*1000;	//Ĭ��3���Ӵ���һ�ε���
			save++;
		}
		
	}
	
	/*-------------------------------------------------------*/
	//���������Ҫ�洢����д���ݵ�eeprom
	if( save != 0 )
	{
		work_data_write();
	}
	
	wdat->net_ptf = 1;
}





