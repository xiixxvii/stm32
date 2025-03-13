#include "work.h"


#define PRT_LOG(X...)	sysprt->alog(X)	
//#define PRT_LOG(X...)	printf(X)	

// ��Сʱ�ͷ���ת��Ϊ�ܷ�����
int to_minutes( uint8_t hour,uint8_t min ) {
		int ret = hour;
		ret = ret * 60 + min;
    return ret;
}



// ����ʱ��ε��ص�������
//start_period ʱ��ο�ʼ�� 				end_period ʱ��ν�����
//start 	 ��Ҫ�����ʱ�俪ʼ�㣬		end   ��Ҫ�����ʱ�������
int calculate_overlap( slot_t period0 ,slot_t period1 ) 
{
    int period_start 	= period0.start;
    int period_end 		= period0.end;

    int time_start 		= period1.start;
    int time_end 			= period1.end;
	
    // ����������
    if ( time_start> period_end ) 
		{
      	period_start 	+= 24 * 60; 	// ����һ��ķ�����
        period_end 		+= 24 * 60; 	// ����һ��ķ�����
    }
		
    if (period_end <= time_start || period_start >= time_end) {
        return 0; // û���ص�
    }
		
    // �����ص��Ŀ�ʼ�ͽ���
    int overlap_start = (period_start > time_start) ? period_start : time_start;
    int overlap_end = (period_end < time_end) ? period_end : time_end;

    return overlap_end - overlap_start; // �����ص��ķ�����
}

//�жϴ����Ǹ�ʱ���
int get_time_segments( slot_t* list,int len,int hour,int min )
{
	int ret = 0;
	uint32_t time = 0;
	uint32_t start = 0;
	uint32_t end = 0;
	time = hour*60+min;
	for( int seg=0;seg<len;seg++ )
	{
		start = list[seg].start;
		end = list[seg].end;
		if( (start <= time) &&( time <= end ) )
		{
			ret = seg;
			break;
		}
		
	}
	
	return ret;
	
}


// �ж��Ƿ�������
int isLeapYear(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ��ȡĳ���µ�����
uint8_t getDaysInMonth(uint16_t year, uint8_t month) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    const uint8_t daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return daysInMonth[month];
}

typedef struct{
	uint32_t Year;
	uint32_t Month;
	uint32_t Date;
	uint32_t Day;			
	uint32_t Hour;
	uint32_t Minute;
	uint32_t Second;
}dt_t;

void addTime( rtc_dt_t *dateTime, int years, int months, int days, int hours, int minutes, int seconds) 
{
	
	dt_t m_dt;
	dt_t *DT = &m_dt;
	
	//��Ϊ rtc_dt_t �ĳ�Ա �����������ޣ�����Ҫ�������Ҫ������ʱ����  DT  ������
	DT->Year 		= dateTime->Year;
	DT->Month 	= dateTime->Month;
	DT->Date 		= dateTime->Date;
	DT->Day 		= dateTime->Day;
	DT->Hour 		= dateTime->Hour;
	DT->Minute 	= dateTime->Minute;
	DT->Second 	= dateTime->Second;	
	
	// �����
	DT->Second += seconds;
	while (DT->Second >= 60) {
			DT->Second -= 60;
			DT->Minute++;
	}
	
	// ��ӷ���
	DT->Minute += minutes;
	while (DT->Minute >= 60) {
			DT->Minute -= 60;
			DT->Hour++;
	}
	
	// ���Сʱ
	DT->Hour += hours;
	while (DT->Hour >= 24) {
			DT->Hour -= 24;
			DT->Date++;
	}

	// �����
	DT->Date += days;
	while (DT->Date > getDaysInMonth(DT->Year, DT->Month)) {
			DT->Date -= getDaysInMonth(DT->Year, DT->Month);
			DT->Month++;
			if (DT->Month > 12) {
					DT->Month = 1;
					DT->Year++;
			}
	}

	// �����
	DT->Month += months;
	while (DT->Month > 12) {
			DT->Month -= 12;
			DT->Year++;
	}

	// �����
	DT->Year += years;
	
	//�����꣬ת��
	dateTime->Year 		= DT->Year;
	dateTime->Month 	= DT->Month;
	dateTime->Date 		= DT->Date;
	dateTime->Day 		= DT->Day;
	dateTime->Hour 		= DT->Hour;
	dateTime->Minute 	= DT->Minute;
	dateTime->Second 	= DT->Second;
}


// ������������ʱ��ת��Ϊ�ܷ�����
uint32_t datetime_to_minutes( rtc_dt_t dt ) 
{
    uint32_t total_minutes = 0;

    // ����������ݵķ�����
    for (int year = 0; year < dt.Year; year++) {
        total_minutes += 365 * 24 * 60; // ÿ�������
        // ��������
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            total_minutes += 24 * 60; // �����һ��
        }
    }

    // ���������·ݵķ�����
    for (int month = 1; month < dt.Month; month++) {
        total_minutes += getDaysInMonth(dt.Year, month) * 24 * 60; // ÿ�·�����
    }
		
    // ���ϵ�ǰ���ڵķ�����
    total_minutes += (dt.Date - 1) * 24 * 60; // ��1�ŵ���ǰ���ڵķ�����
    total_minutes += dt.Hour * 60;            // ��ǰСʱ�ķ�����
    total_minutes += dt.Minute;                // ��ǰ������

    return total_minutes;
}

// �����������ȥ�˶��ٷ���
uint32_t calculate_minutes_difference( rtc_dt_t current, rtc_dt_t past) 
{
    uint32_t current_minutes = datetime_to_minutes(current);
    uint32_t past_minutes = datetime_to_minutes(past);
    return current_minutes - past_minutes; // ���ز�ֵ
}



void dump_rtc_by_str( char* time )
{
	int i=0;
	uint8_t tbuf[20];
	uint32_t year,month,date,hour,min,second;

	//�� 0123
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<4;i++ )	{tbuf[ i ] = time[ 0 + i ];}
	year = atol( (char*)tbuf );
	
	//�� 45
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 4 + i ];}
	month = atol( (char*)tbuf );
	
	//�� 67
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 6 + i ];}
	date = atol( (char*)tbuf );
	
	//ʱ 89
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 8 + i ];}
	hour = atol( (char*)tbuf );
	
	//�� 10 11
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 10 + i ];}
	min = atol( (char*)tbuf );
	
	//�� 12 13
	memset( tbuf,0,sizeof(tbuf) );
	for( i=0;i<2;i++ )	{tbuf[ i ] = time[ 12 + i ];}
	second = atol( (char*)tbuf );
	
	sysprt->alog("%d/%d/%d - %d:%d:%d\r\n",year,month,date,hour,min,second );
					
	rtc_dt.Year = year;
	rtc_dt.Month = month;
	rtc_dt.Date = date;
	rtc_dt.Hour = hour;
	rtc_dt.Minute = min;
	rtc_dt.Second = second;
	
	rtc->stop();
	rtc->init( &rtc_dt,0,1 );
	rtc->start();
	
}




