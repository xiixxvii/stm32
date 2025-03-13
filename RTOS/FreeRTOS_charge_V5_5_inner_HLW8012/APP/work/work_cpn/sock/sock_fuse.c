#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	



/*---------------------------------------------------------------------*/
//����˿����
static int fuse_err[MaxSock] = {0,0};

static uint32_t boot_sec = 0;	//��¼�����������˶��ٸ�5��


int cat_fuse_err( int road )
{
	
	//���ټ����20s,���ܷ�ӳ����˿�����
	if( boot_sec>= 20 )
		return fuse_err[road];
	else
		return 0;
}

static uint8_t PinMapFuse[MaxSock] = {IO_fuse0,IO_fuse1};

void listen_fuse( void )
{
	static time_t otick_fuse;
	static time_t ntick_fuse;
	static uint32_t cntr[MaxSock] = {0,0};
	static uint32_t sta [MaxSock] = {0,0};
	uint32_t val;
#if ( BoardVision == BoardV5_4)
	//�ɰ���ֻ��һ������˿����0Ϊ׼
	val = gpio->read( PinMapFuse[0] );
	switch( sta[0] )
	{
		case 0://�ȴ�0
			if( 0 == val ) 	
			{
				sta[0]++;
			}
			break;
		case 1://�ȴ�1
			if( 1 == val )
			{
				cntr[0] ++;
				cntr[1] ++;
				sta[0]--;
			}
			break;
		default:
			sta[0]=0;
			break;
		
	}
	
#else
	
	for( int road=0;road<MaxSock;road++ )
	{
		val = gpio->read( PinMapFuse[road] );
		switch( sta[road] )
		{
			case 0://�ȴ�0
				if( 0 == val ) 	{sta[road]++;}
				break;
			case 1://�ȴ�1
				if( 1 == val )
				{
					cntr[road] ++;
					sta[road]--;
				}
				break;
			default:
				sta[road]=0;
				break;
			
		}
	}
#endif
	ntick_fuse = get_sys_ticks();
	
	if( 10000 >( ntick_fuse - otick_fuse ) )	return ;
	otick_fuse = ntick_fuse;
	
	//��¼����������ʱ��,ÿ����һ�죬���ü�����
	boot_sec = boot_sec + 5;
	if( boot_sec >24*60*60*1000 ) boot_sec = 0;
	
//	sysprt->talog("listen_fuse:%%010u \r\n",cntr );
	
	val = 0;
	for(int road=0;road<MaxSock;road++)
	{
			if( cntr[road] <20 )
			{
				val ++;
				fuse_err[road] = 1;
				sysprt->talog("[%d]listen_fuse:%010u \r\n",road,cntr[road] );
			}
			else
			{
				fuse_err[road] = 0;
			}
			cntr[road] = 0;
	
	}
	
	//ָʾ��
	if( val>0 )
	{
		led_board->set_fuse_led( __OFF );
	}
	else
	{
		led_board->set_fuse_led( __ON );
	}
	
	return;
}


/*----------------------------------------------------------------------------------*/



