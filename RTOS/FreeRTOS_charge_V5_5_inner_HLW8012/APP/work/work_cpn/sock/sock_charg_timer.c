#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

//���¶����ѹ�ʱ��
void utime_refresh( sock_sta_pt pssock,time_t tick  )
{
	//�����ѹ�ʱ��
	int id = 0;
//	pssock->utick_order = tick - pssock->otick_order +  pssock->order.utime;
	uint32_t stime = datetime_to_minutes( guide_dat->order_info->sock[id].start );
	uint32_t ntime = datetime_to_minutes( rtc_dt );
	
	pssock->utime_order = ntime - stime;
	
}





