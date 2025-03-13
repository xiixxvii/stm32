
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

//----------------------------------------
/*���翴�Ź���ʱ��
��Ping��ʱ��ʼ��ʱ��
�������һ��ʱ�䣬û���յ�pong������Ϊ�п��ܵ����ˣ������һ��ͨѶ����
�������������һ��ֵ����˵������ȷʵ������

*/


static time_t otick=0,ntick=0;
static uint32_t err=0;



static void feed( wk_inet_pt inet )
{

	otick = get_sys_ticks();
	err = 0;
}





static int deal( wk_inet_pt inet )
{
	
	int ret = 0;
	int dump_len = 0;
	int sock_sta = 0;
	
	uint8_t tbuf[128];

	ntick = get_sys_ticks();
	
	if( ( ntick - otick )<10000 ) return 0;
	otick = ntick;
	
	
	err++;
	if( err >= 5 )
	{//˵��������
//		PRT_LOG( "[%d/%d/%d-%d:%d:%d]err:PING no pong( %d ) \r\n",get_sys_ticks(),err );
		PRT_LOG( "[%4d/%2d/%2d-%2d:%2d:%2d]err:PING no pong( %d ) \r\n",
			rtc_dt.Year,rtc_dt.Month,rtc_dt.Date,rtc_dt.Hour,rtc_dt.Minute,rtc_dt.Second,err );
		//��λ�������ģ��
		inet->msta = 0;
		inet->ssta = 1;
		return -1;
	}
	//����PING
	if( wdat->net_ptf )
		PRT_LOG( "[%04d/%02d/%02d-%02d:%02d:%02d]PING( %d ) \r\n",
			rtc_dt.Year,rtc_dt.Month,rtc_dt.Date,rtc_dt.Hour,rtc_dt.Minute,rtc_dt.Second,err );

//		PRT_LOG( "[%010u]PING( %u )\r\n",get_sys_ticks(),err );
	/*----------------------------------------------------------------------------------*/
	
	memset( tbuf,0,sizeof(tbuf) );
	ret = strlib->dump( (char*)(&tbuf[ dump_len ]),"PING " );
	dump_len += ret;
	
	//������0��״̬
	for( int road=0;road<MaxSock;road++ )
	{
		sock_sta = get_sock_sta( road );
		switch(sock_sta)
		{
			case 0x00:	ret = strlib->dump( (char*)(&tbuf[ dump_len ]),"00 " );	break;
			case 0x02:	ret = strlib->dump( (char*)(&tbuf[ dump_len ]),"02 " );	break;
			case 0x40:	ret = strlib->dump( (char*)(&tbuf[ dump_len ]),"40 " );	break;
			default:	break;
		}
		
		dump_len += ret;
	}
	
	//��䵱ǰ�Ļ�����ѹ
	ret = strlib->dump( (char*)(&tbuf[ dump_len ]),"220\r\n" );
	dump_len += ret;
	
	
//	air_man->send_str( air724, "PING\r\n" );
	
	show_send_data( (char*)tbuf );
	
	air_man->send_str( air724, (char*)tbuf );
	/*----------------------------------------------------------------------------------*/
#if 0
	//��ӡ��ʱʱ��
	uint16_t shour = 0,smin=0;
	uint16_t ehour = 0,emin=0;
	if( guide_dat->time_slot->get_slot )
		for( int i=0;i<guide_dat->time_slot->segments;i++ )
		{
			shour = guide_dat->time_slot->val[i].start/60;
			smin = guide_dat->time_slot->val[i].start%60;
			ehour = guide_dat->time_slot->val[i].end/60;
			emin = guide_dat->time_slot->val[i].end%60;
			
			PRT_LOG("group=%2d,start=%02d:%02d,end=%02d:%02d\r\n",
				guide_dat->time_slot->val[i].group,
				shour,smin,ehour,emin
			);
			
		}
#endif
	
	
	return 1;
}

static nwdt_t m_nwdt = {
	.feed = feed,
	.deal = deal,
};

nwdt_pt nwdt = &m_nwdt;

