
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

static uint8_t http_date[20];
char* get_date_str(void)
{
	return (char*)http_date;
}


static int send_get_time(void);


static void air724_connect_internet( wk_inet_pt inet );
//------------------------------------------------------------------------------

void work_internet_http_date( wk_inet_pt inet )
{
//	static time_t otick=0,ntick=0;
	switch( inet->ssta )
	{
		case 0:
			air724_connect_internet( inet );

			inet->msta++;
			inet->ssta=0;
			
			break;
		
		default:
			break;
	}
	
}

//------------------------------------------------------------------------------

#define CMD_MaxErr	5
#define __ERR_AIR_RST( ) 	{ line = __LINE__;goto __AIR_RST; }



static void air724_connect_internet( wk_inet_pt inet )
{
	
	char* argv[5];
	
	int err = 0,ret = 0,line = 0;

		
	air_dev_pt  this_dev;
	this_dev = air724;
	
	//����TCP����
	argv[0] = "TCP";							// TCP/UDP
	argv[1] = serv_ip;		// IP addr
	argv[2] = "9000";								// port
	
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPSTART( this_dev,3,argv );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		task_delay(3000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	//�ȴ��յ�CONNECT
//	while(1)
//	{
//		
//		ret = air724->cmd_wait_CONNECT( this_dev,3,argv );
//		air_man->ptf_rxbuf( this_dev,NULL );
//		if( ret == 1 ) break;
//		task_delay(200);
//		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
//	}
	
	thread_delay_ms( 1000 );
	air_man->clr_rxbuf( this_dev );
	
	PRT_LOG("\r\n get_time: \r\n");
	err = 0;
	while( 1 )
	{		
		ret = send_get_time( );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		task_delay(2000);
		if( (err++)>2 ){__ERR_AIR_RST();}
	}
	PRT_LOG("\r\n get_time success \r\n");
	
	//�ر�TCP����
//	err = 0;
//	while( 1 )
//	{		
//		ret = air724->cmd_AT_CIPCLOSE( this_dev,NULL,NULL );
//		air_man->ptf_rxbuf( this_dev,NULL );
//		if( ret >= 0 ) break;
//		task_delay(2000);
//		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
//	}
//	PRT_LOG("\r\nclose tcp of http \r\n");
	
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
static uint8_t  temp_buf[128];

static int send_get_time(void)
{
	

	
	air_dev_pt  this_dev;
	this_dev = air724;
	
	int i = 0;
	memset( temp_buf,0,sizeof( temp_buf ) );
	int dump_len = 0;
	int ret = 0;
	
	uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len
	uint8_t* rxbuf = this_dev->rxbuf;							//���ջ�����ָ�룬ָ��ᱻ�޸�
//	uint32_t err_cntr = 0;										//������
	
	ret = strlib->dump( (char*)(&temp_buf[ dump_len ]),"GETTIME\r\n" );
	dump_len += ret;
	
	memset( rxbuf,0,1024 );
	air_man->clr_rxbuf( this_dev );
	//����
	air_man->send_str( this_dev, (char*)temp_buf );

	memset( temp_buf,0,sizeof( temp_buf ) );
	this_dev->rxlen =0;
	thread_delay_ms(1500);
	while(1)
	{
		//�����ݵ�dev->rxbuf
		rd_len = this_dev->uart->read_recv_buf( &(rxbuf[ this_dev->rxlen ]),1 );

		this_dev->rxlen += rd_len;
		
		//����û���յ��κ�����
		if( rd_len == 0  ) 
		{
			if( strlib->findstr( (char*)rxbuf,"\r\n" ) >= 0 )
			{
				sysprt->alog("find entr\r\n");
				
				ret = strlib->findstr( (char*)rxbuf,"20" );
				if( ret >= 0 )
				{
					sysprt->alog("find 20->%d \r\n",ret );
					sysprt->alog("rxbuf:%s",rxbuf);
					
					//�������ַ������Ƴ���,���ڼ���
					for( i=0; i<8; i++ )
					{
						http_date[ i ] = rxbuf[ ret + i ];
					}
					http_date[i] = '\0';					
					PRT_LOG("http_date:%s\r\n", http_date );
					
					//�������շ֡�ʱ����ֱ����������ʱֱ���͸�RTC��
					//�Ժ�Ҫ�ڳɹ���¼����͸�RTCȷ������OK
					dump_rtc_by_str( (char*)(&(rxbuf[ret])) );
					
					
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
			
			
		}
	}
	
}

//------------------------------------------------------------------------------

