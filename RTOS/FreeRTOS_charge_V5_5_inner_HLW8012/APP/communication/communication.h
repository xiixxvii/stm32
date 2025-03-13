#ifndef __communication_h__
#define __communication_h__

#include "framework.h"

/*---------------------------------------------------------*/
//��װ
typedef struct{
	void (*cycle_report_set)(uint8_t enable);							//��ʱ�㱨 ���ܿ���
	void (*cycle_report )(time_t tick);										//��ʱ�㱨 ִ�к���
	void (*event_report_set)(uint8_t pin,uint8_t event);	//�¼��㱨 ���ܿ���
	void (*event_report)(void);														//�¼��㱨 ִ�к���
}inb_report_t,*inb_report_pt;
/*---------------------------------------------------------*/
//��׼����֡
typedef struct{
	uint8_t sCode;					//֡ͷ
	uint8_t cCode;					//������
	uint8_t dat[8];					//���ݿ�
	uint8_t sum;						//У���
	uint8_t eCode;					//֡β
	
}cframe_t,*cframe_pt;
/*---------------------------------------------------------*/

typedef void (*cmd_exec_t)(void* frame);

typedef struct{
	uint16_t num;										//���
	uint16_t usage;									//ʹ�����
	uint32_t cCode;									//������
	cmd_exec_t exec;								//ִ��
	
}cmd_t,*cmd_pt;

typedef struct{
	int (*reg)( uint32_t cCode, cmd_exec_t exec );
	int (*unreg)( uint32_t cCode );
	void (*deal)( void* rframe );
}cmd_man_t,*cmd_man_pt;
/*---------------------------------------------------------*/

typedef struct{
	void (*init_dev)( uart_pt uart );
	void (*set_volume)(uart_pt uart, uint8_t vol);
	void (*set_gingle_cycle)(uart_pt uart );
	void (*play_now)( uart_pt uart );
	void (*play_music)( uart_pt uart, uint8_t val);
	
}audio_opt_t,*audio_opt_pt;


/*---------------------------------------------------------*/
//�ӿ�
extern cmd_man_pt cmd_man;

extern inb_report_pt inb_report;

extern audio_opt_pt audio_opt;

extern void cmd_analysis_handle(void);
/*---------------------------------------------------------*/
#define cmd_len 			12
#define cmd_start			0xA5
#define cmd_end				0x5A

#define cmd_max_num		10
#define _isEmpty			0
#define _isUsed				1

#define __cCode_of_CtrlConf			0x00
#define __cCode_of_Heartbeat		0x01
#define __cCode_of_OutBusCtrl		0x02
#define __cCode_of_InBusRpt			0x03
#define __cCode_of_AudioDev			0x04



#endif


