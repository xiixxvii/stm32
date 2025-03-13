#ifndef __work_h__
#define __work_h__

/*���ŵ�ʹ��




*/


#define IV_Mode	1
// =0,�ɰ� ���ԳƵľɵ�·��
// =1,�°� �Գ���ƣ����ǲ����ͼ��ͬʱ���ҵߵ���



#define App_Version                      5               /**< App major version number */
#define App_SubVersion                   5               /**< App minor version number */




/*
{
����app�汾
	20230523: 3-2
}

*/



extern char version_info[20];

extern void req_goto_update( void );


#include "framework.h"
#include <flashdb.h>
/*-------------------------------------------------------*/
#define BOOT_CODE 0xA5				//�����룬�����bootloaderһ��
#define NetMode_Not			0
#define NetMode_Air724	1
#define NetMode_Wifi		2
#define NetMode_nbiot		3
#define NetMode_Air780	4


enum _program{
	_app_update = 0,
	_app_fac,
	_app_new
};

enum _ota_server {
	_ota_ydc_serv = 0,	//�ѵ³������
	_ota_test_serv,			//���Է�����
};

//#define WorkIngCode		0xD0			
//typedef struct{
//	uint32_t work_code;			//������	 =WorkIngCode,˵����һ�ζ���δ����
//	uint32_t tick;			//ʣ��ʱ��
//	char ddh[20];				//������
//	
//}last_order_t,*last_order_pt;


typedef struct{
	uint32_t init_code; //������	 =BOOT_CODE,˵���Ѿ���ʼ����
	uint32_t tick_put_curr;			//��ʱ�������������
	uint32_t res[2];						//���㣬���뵽16����
	
	
}app_dat_t,*app_dat_pt;

typedef struct{
	uint8_t boot_code;		//������
	uint8_t program;			//ѡ������ʲô����
	uint8_t net_mode;			//������ʽ
	uint8_t rp_boot;			//�ϱ���������һ������	
	
	uint32_t net_ptf;			//�������ݴ�ӡ
	
	uint32_t zd_val;			//�ն˱��
	double curr_ratio[2]; //����ϵ��
	
	float k[2];					//У׼������ֱ��ϵ��k
	float b[2];					//У׼������ֱ�߽ؾ�b
	
	uint32_t ota_server;	//ota����������
	
	uint32_t res[2];		//���㣬���뵽16���ֽ�
	
	/*----------------------------------------------*/
	//�·���appר������
	app_dat_t app_dat;	//Ϊ�˼��ݾɻ�����bootloader,��app1����app_dat
	
	
}wdat_t,*wdat_pt;
extern wdat_pt wdat;
extern void work_data_load( void );
extern void work_data_write( void );

/*-------------------------------------------------------*/
#define MaxSock 2


typedef struct{
//	wk_num_pt num;	//������Ϣ
	int msta;
	int ssta;
	
}wk_inet_t,*wk_inet_pt;


/*-------------------------------------------------------*/

/*-------------------------------------------------------*/
//�µ�work dat


//----
typedef struct{
	uint32_t code;					//������					=0xA5
	uint32_t cntr;					//����������
}boot_sta_t;

#define MaxSolt	24		//���֧��24��ʱ���
#define MaxPeriodE 6	//���֧��6���������

typedef struct{
	uint32_t group;			//����������һ���ʱ��
	uint32_t start;			//��λ ����
	uint32_t end;				//��λ ����
}slot_t;

/*

*/
typedef struct{
	uint8_t get_date;			//�����Ƿ��Ѿ���ȡ����ʱ��
	uint8_t get_slot;			//�����Ƿ��Ѿ���ȡ��ʱʱ��
	uint8_t segments;			//һ���ж��ٸ��� 
	uint8_t groups;				//һ���ж�����
	slot_t val[MaxSolt];	//���ε�����
}time_slot_t;


//----
typedef struct{
	uint32_t pul[2];		//����������
}total_e_t;
//----

typedef struct{
	uint32_t sock[2];		//�����������
}rtd_num_t;
//----
#define MaxDDH_Len 32
typedef struct{
	
	uint8_t working;							//���������Ƿ����ڹ��� =0,������ =1 ���ڹ���
	uint8_t rec[3];								//����
	
	//������
	char ddh[MaxDDH_Len];	
	
	//��ʼʱ��
	rtc_dt_t start;								//������ʼʱ��
	rtc_dt_t end;									//�ƻ�����ʱ��
	
	uint32_t pluse[MaxSolt];			//�������
	
	
}order_dat_t;
//----
typedef struct{
	order_dat_t sock[2];		//����������
}order_info_t;

//typedef uint32_t dval_t;
/*-------------------------------------------------------*/
//���д���
typedef struct{
	
	//��Ҫ�洢������
	boot_sta_t* boot_sta;
	total_e_t* total_e;						//�����ܵ���
	rtd_num_t* rtd_num;						//ʵʱ�������к�	
	order_info_t* order_info;			//����������Ϣ

	//���洢�����ݣ�ֻ����
	time_slot_t* time_slot;				//24��ʱ���,���ûʹ�ã���Ϊ0

	
}wdat_guide_t;

extern wdat_guide_t *guide_dat;

/*-------------------------------------------------------*/
typedef struct{
	int (*get_by_fdb)( void );
	int (*set_to_fdb)( void* dat );
	
}db_base_t;

typedef struct{
	db_base_t boot_sta;
	db_base_t total_e;
	db_base_t rtd_num;
	db_base_t order_info;
	
}wdat_opt_t,*wdat_opt_pt;

extern wdat_opt_pt wdat_opt;
extern void wdat_guide_init(void);
/*-------------------------------------------------------*/


// ����ʱ��ε��ص�������
extern int calculate_overlap( slot_t period0 ,slot_t period1 ) ;
//�жϴ����Ǹ�ʱ���
extern int get_time_segments( slot_t* list,int len,int hour,int min );
//����������ʱ��
extern void addTime( rtc_dt_t *dateTime, int years, int months, int days, int hours, int minutes, int seconds);
// ������������ʱ��ת��Ϊ�ܷ�����
extern uint32_t datetime_to_minutes( rtc_dt_t dt );
// �����������ȥ�˶��ٷ���
extern uint32_t calculate_minutes_difference( rtc_dt_t current, rtc_dt_t past) ;

//ͨ���ַ������rtc �ַ�����ʽ ������ʱ���� 20241025102210
extern void dump_rtc_by_str( char* time );

extern void work_internet(void);

extern void prt_date_time( rtc_dt_t* dt,char* str );

extern void dump_msh_info( char* num,char* date );

extern uint32_t get_msh_terminal_val( void );
extern char* get_msh_terminal_str( void );

extern char *serv_ip;


extern void save_order_them_shutdown( void );		//����֮ǰ�����ô˳���


#include "./work_cpn/work_cpn_typedef.h"

#include "./work_cpn/work_cpn_inc_conf.h"


#endif

