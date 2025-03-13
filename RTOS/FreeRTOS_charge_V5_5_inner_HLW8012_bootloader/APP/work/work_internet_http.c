#include "task_inc.h"
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)
//------------------------------------------------------------------------------

char* ota_serv_ip[2] ={
	"14.18.45.140",				// IP addr ��˾���׮�ķ�����
	"124.223.100.141"			// IP addr linjie�ķ�����
};
char* http_serv = NULL;

typedef struct{
	uint32_t file_size;		//�����ļ��ߴ�
	uint32_t get_flag;		//�����ļ��ߴ���Ϣ�Ѿ���ȡ
	
	uint32_t recv_size;		//������ǰ�����˶����ֽ�
	uint32_t part_err;		//��Ƭ�������������ֵԽ���������ٶ�Խ��
	
	time_t start_tick;		//��ʼ����ʱ�ĵδ�
}file_info_t,*file_info_pt;

file_info_t file_info ={0,0,0};

//------------------------------------------------------------------------------
typedef struct{
	uint32_t wlen;	//�Ѿ�д��ĳ���
	uint32_t waddr;	//��ǰд��ĵ�ַ
}wapp_t,*wapp_pt;

static wapp_t m_wapp;
static wapp_pt wapp = &m_wapp;
static void program_app1_init( wapp_pt wapp )
{
	wapp->wlen = 0;
	wapp->waddr = addr_app1;//0x50000;
	PRT_LOG("program_init\r\n");
	for( int i=0;i<192/4;i++ )	//����ռ���192KB,ÿ�β���4KB
	{
		FLASH_Erase( wapp->waddr + i*4096 );
	}
	PRT_LOG("->\r\n");
	
}
static void program_app1( wapp_pt wapp,uint8_t* buf,uint32_t len )
{
	FLASH_Write( wapp->waddr,(uint32_t*)(buf), len/4 );
	wapp->waddr += len;
}
//------------------------------------------------------------------------------

extern int http_download_get_size( void* airdev,uint32_t *size );
extern int http_download_part( void* airdev,uint8_t *dbuf,uint32_t vstart,uint32_t vend ,uint32_t speed );
extern int http_close( void* airdev );





static void work_http_connect_internet( wk_inet_pt inet );
static void work_http_get_file_size( wk_inet_pt inet );
static void work_http_get_file_part( wk_inet_pt inet );
static void work_http_close( wk_inet_pt inet );

static void work_http_set_app_to_new( wk_inet_pt inet );

//------------------------------------------------------------------------------

void work_internet_http( wk_inet_pt inet )
{
//	static time_t otick=0,ntick=0;
	switch( inet->ssta )
	{
		case 0:	
			work_http_connect_internet( inet );
			break;
		
		case 1:
			work_http_get_file_size( inet );
			break;
		
		case 2:
			work_http_get_file_part( inet );
			break;
		
		case 3:
			work_http_close( inet );
			break;
		
		case 4:
			work_http_set_app_to_new( inet );
			break;
		
		default:
			break;
	}
	
}

//------------------------------------------------------------------------------

#define CMD_MaxErr	3
#define __ERR_AIR_RST( ) 	{ line = __LINE__;goto __AIR_RST; }

//------------------------------------------------------------------------------

static void work_http_connect_internet( wk_inet_pt inet )
{
	
	char* argv[5];

	int err = 0,ret = 0,line = 0;
		
	air_dev_pt  this_dev;
	this_dev = air724;
	
	//����TCP����
	argv[0] = "TCP";							// TCP/UDP
	
	// IP addr
	http_serv = ota_serv_ip[ wdat->ota_server ];

//	switch( wdat->ota_server )
//	{
//		case _ota_ydc_serv:
//			http_serv = ota_serv_ydc;
//			break;
//		
//		case _ota_test_serv:
//			http_serv = ota_serv_test;
//			break;
//		default:
//			http_serv = ota_serv_ydc;
//			break;
//	}
	argv[1] = http_serv;					// IP addr
	
	argv[2] = "80";								// port
	
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPSTART( this_dev,3,argv );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		task_delay(2000);
		if( (err++)>2 ){__ERR_AIR_RST();}
	}
	
	inet->ssta++;
	
	return ;
//	air_man->send_str( this_dev,"+++" );
__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	//����λ
	inet->msta = 0;
	inet->ssta = 1;
	
	return;
}

//------------------------------------------------------------------------------

static uint32_t vstart = 0;



static void work_http_get_file_size( wk_inet_pt inet )
{
	int err = 0,ret = 0,line = 0;
	air_dev_pt  this_dev;
	this_dev = air724;
	
	uint32_t file_size = 0;

	
	//����Ѿ���ȡ���ļ���Ϣ����ֱ�ӽ�����һ��
	if( file_info.get_flag ) goto __Next_ssta;
	
	
	//http ������������� �ߴ�
	err = 0;
	while( 1 )
	{		
		ret = http_download_get_size( this_dev,&file_size );
//		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(200);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}

	file_info.file_size = file_size;
	file_info.get_flag 	= 1;
	file_info.recv_size = 0;
	file_info.part_err 	= 0;
	file_info.start_tick = get_sys_ticks();
	
	program_app1_init( wapp );
	
__Next_ssta:
	PRT_LOG("\r\n http_download_get:\r\n");
	PRT_LOG("\tfile name:%s\r\n",http_file_path)	;
	PRT_LOG("\tfile size:%s\r\n",file_info.file_size)	;

	
	inet->ssta++;
	
	return;
__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	//����λ
	inet->msta = 0;
	inet->ssta = 1;
	
	return;
}

//------------------------------------------------------------------------------

static uint16_t crc16_ibm(const void* data, size_t size)
{
    const uint8_t* bytes = (uint8_t*)data;
    uint16_t crc = 0;

    while (size--) {
        crc ^= *bytes++;
        for (int i = 0; i < 8; ++i) {
            crc = (crc >> 1) ^ ((crc & 0x01) ? 0xA001 : 0);
        }
    }

    return crc;
}

//http ���ط�Ƭ

static uint8_t dbuf[1024];
static void work_http_get_file_part( wk_inet_pt inet )
{
		int err = 0,ret = 0,line = 0;
	air_dev_pt  this_dev;
	this_dev = air724;
	
	uint16_t crc = 0;
	

	
	while( 1 )
	{
		err = 0;
		vstart = file_info.recv_size;
		
		while( 1 )
		{
			//ÿ����130�ֽڣ�128+2�ֽ�CRC IBM
			ret = http_download_part( this_dev,dbuf,vstart,vstart+129, file_info.part_err );
//			air_man->ptf_rxbuf( this_dev,NULL );
			if( ret == 1 ) break;
			
			file_info.part_err++;
			
			
			PRT_LOG( "\r\n[err%d]\r\n",file_info.part_err);
			thread_delay_ms(200);
			if( (err++)>1 ){__ERR_AIR_RST();}
		}
		//���Ϸ���,���Ϸ������������
		crc = crc16_ibm( dbuf, 128 );
		
//		PRT_LOG( "crc[  0]= %02x,crc[  1]=%02x\r\n",(uint8_t)crc,(uint8_t)(crc>>8) );
//		PRT_LOG( "buf[128]= %02x,buf[129]=%02x\r\n",dbuf[128],dbuf[129] );
		if(  
				( ( (uint8_t)crc 			) == dbuf[128] ) &&
				( ( (uint8_t)(crc>>8) ) == dbuf[129] )
			)
		{//˵���յ��������ǺϷ���
			//�Ϸ�����д��
			program_app1( wapp,dbuf,128 );
			//��������
			file_info.recv_size += 130;
			
			
			
			uint32_t dtick = get_sys_ticks()-file_info.start_tick;
			uint32_t speed = file_info.recv_size / (dtick / 1000);
			uint32_t now = file_info.recv_size / ( file_info.file_size/100 );
			
			PRT_LOG( "\r\n[%02d%%]/[%08ds]/[%dB/s][%d/%d][wdt %d/%d]\r\n",
				now, dtick/1000,speed,file_info.recv_size,file_info.file_size,
				ota_wdt_tick(),OTA_TimeOut
			);
			
			if( file_info.recv_size >= file_info.file_size )
			{
				PRT_LOG( "recv success \r\n");
				
				break ;
			}
		}
		else
		{//˵���յ��������ǷǷ���
			PRT_LOG( "error:crc\r\n" );
			file_info.part_err++;
			continue;
		}

	}
	
	inet->ssta++;
	return;


__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	//��������
	inet->ssta = 0;
	
	ret = air724->cmd_AT_CIPCLOSE( this_dev,NULL,NULL );
	
	return;
}

//------------------------------------------------------------------------------
//http �ر�
	
static void work_http_close( wk_inet_pt inet )
{
	
//	int err = 0,ret = 0,line = 0;
	air_dev_pt  this_dev;
	this_dev = air724;


	//�ر�TCP����
//	err = 0;
//	while( 1 )
//	{		
//		ret = air724->cmd_AT_CIPCLOSE( this_dev,NULL,NULL );
//		air_man->ptf_rxbuf( this_dev,NULL );
//		if( ret >= 0 ) break;
//		thread_delay_ms(2000);
//		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
//	}
	http_close( this_dev );
	PRT_LOG("\r\nclose tcp of http \r\n");
	inet->ssta++;
	
	return ;
//__AIR_RST:
//	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
//	//����λ
//	inet->msta = 0;
//	inet->ssta = 1;
	
//	return;
}
//------------------------------------------------------------------------------
extern void req_rst_mcu( void );
static void work_http_set_app_to_new( wk_inet_pt inet )
{
	wdat->program = _app_new;
	wdat->rp_boot = 1;						//˵�����ο���Ҫ�����������
	work_data_write();
	PRT_LOG("work_http_set_app_to_new\r\n");
	
	req_rst_mcu();
	thread_delay_ms(10000);
	
}


