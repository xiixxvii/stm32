#ifndef __work_h__
#define __work_h__




#include "framework.h"
/*-------------------------------------------------------*/
#define BOOT_CODE 0xA5
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
	
	uint32_t res[2];		//���㣬���뵽16����
		
}wdat_t,*wdat_pt;
extern wdat_pt wdat;
extern void work_data_load( void );
extern void work_data_write( void );
/*-------------------------------------------------------*/
#define MaxSock 2

//typedef struct{
//	uint8_t val[3];	//�豸��
//	uint8_t sum;		//�豸�ż����
//}wk_num_t,*wk_num_pt;

typedef struct{
//	wk_num_pt num;	//������Ϣ
	int msta;
	int ssta;
	
}wk_inet_t,*wk_inet_pt;

#include "./work_cpn/work_cpn_typedef.h"




#include "./work_cpn/work_cpn_inc_conf.h"


extern void work_internet(void);



extern void dump_msh_info( char* num,char* date );

extern uint32_t get_msh_terminal_val( void );
extern char* get_msh_terminal_str( void );



#endif

