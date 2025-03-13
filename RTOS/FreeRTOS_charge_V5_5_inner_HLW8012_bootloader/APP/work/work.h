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
	_ota_ydc_serv = 0,	//友德充服务器
	_ota_test_serv,			//测试服务器
};

typedef struct{
	uint8_t boot_code;		//启动码
	uint8_t program;			//选择启动什么程序
	uint8_t net_mode;			//联网方式
	uint8_t rp_boot;			//上报服务器第一次启动	
	
	uint32_t net_ptf;			//网络内容打印
	
	uint32_t zd_val;			//终端编号
	double curr_ratio[2]; //电流系数
	
	float k[2];					//校准后的拟合直线系数k
	float b[2];					//校准后的拟合直线截距b
	
	uint32_t ota_server;	//ota服务器配置
	
	uint32_t res[2];		//补足，对齐到16个字
		
}wdat_t,*wdat_pt;
extern wdat_pt wdat;
extern void work_data_load( void );
extern void work_data_write( void );
/*-------------------------------------------------------*/
#define MaxSock 2

//typedef struct{
//	uint8_t val[3];	//设备号
//	uint8_t sum;		//设备号检验和
//}wk_num_t,*wk_num_pt;

typedef struct{
//	wk_num_pt num;	//机器信息
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

