
typedef struct{
	char ddh[MaxDDH_Len];	//������
	uint16_t num;	//����������һ���豸
	uint16_t cmd;	//������������
	uint32_t dat;	//��������������ʣ����ó��ʱ��,��λ����
//	uint32_t utime;	//������������������ʱ��
	uint32_t param;	//������������������ʱ��
}order_t,*order_pt;


#define _event_curr 			0			//������־
#define _event_sock   		1			//����������־
#define _event_net				2			//������־
#define _event_recore			3			//������־
#define _event_m_tenance 	4			//ά����־

//�������
typedef struct{
	uint32_t min;		//���ٷ���
	float kwh;			//ǧ��ʱ
}period_e_t;

typedef struct{
	char ddh[MaxDDH_Len];		//������
	uint32_t type;	//�����¼�����	=0 ������=1 �����¼�
	
	uint16_t num;		//����������һ���豸
	uint16_t val;		//�����¼�ֵ
	
	uint32_t dat;		//������������
	double curr;		//��������ֵ����
	
	period_e_t period_e[MaxPeriodE];	//������������ʱ���������
	
	
}event_t,*event_pt;


typedef struct{
	void (*init)( void );
	void (*send_order)( char* ddh, int num, int cmd, uint32_t dat, uint32_t param  );
	void (*put_event)( char* ddh,int type,int num, 
					int val, uint32_t dat,double curr,void* period_e   );
	int (*recv_order)( order_pt order );
	int (*recv_event)( event_pt event );

}sock_mq_t,*sock_mq_pt;

enum sock_order{//order cmd to 
	oct_start_charge = 0,	//���
	oct_cancel_charge,		//ȡ�����
	oct_clr_err,					//�������
};

enum sock_event{//order event by
	oeb_time = 0, 	 						//������������
	oeb_curr_too_small,					//������С
	oeb_curr_too_big,						//��������
	oeb_no_ins,									//û�в�������
	oeb_fuse_broken,						//����˿�ϵ���
	oeb_manual_stop,						//�ֶ�������������ȴ���������
	
};

/*---------------------------------------------------------------*/
//hlw8032

//typedef struct{
//	uint32_t SOCK_CONNECT;	//�����Ƿ���豸
//	double SOCK_P;					//��Ч����
//	double SOCK_Vol;				//������ѹ
//	
//}sock_hlw_dat_t,*sock_hlw_dat_pt;

#define MaxHLW	2


#define cmd_start0 	0x5A
#define cmd_len			24


//��������״̬����
typedef struct{
	int msta;
	int ssta;
	
	int resver;
	
	uint32_t otick_order;			//����������ʼʱ��		��λ ms
//	uint64_t dtick_order;			//��������Ŀ��ʱ��
	
	uint32_t utime_order;			//�����ѹ�ʱ��(��ֵ�ڳ������У���ʱ����) ��λ min
	uint32_t atime_order;			//�����ܵ�ʱ�� ��λ min
	
	float base_curr;						//���ڶ�ʱ�ϴ�ʱ�����仯�̶ȵĻ�������
	
	float average_curr;				//N�������ڵ�ƽ������
	float average_pwr;					//N�������ڵ�ƽ������
	
	float min_w_s;							//���1���������ĵĵ��� ���� w * s
	
	
	uint16_t sock_out;			//�������γ�
	uint16_t cntr_out;			//�������γ�������
	uint32_t tick_out;			//�������γ��¼�
	
	int order_remind;					//�����Ƿ��ж�������
	
	uint16_t order_stop_req;	//�����Ƿ���ڶ�����������
	uint16_t big_cntr;				//�����������
	
	order_t order;						//��ǰ��������(���߱�ʵʱ��)
	
}sock_sta_t,*sock_sta_pt;

/*----------------------------------------------------------------------------------*/

enum{
	_type_hlw_sock_out = 0,	//�β���
	_type_hlw_cyc_pul,			//��������
	_type_hlw_min_pul,			//1��������
	
};

typedef struct{
	uint32_t type;		//��������
	uint32_t cyc;			//��������
	uint32_t val;			//������������
}hlw_pul_t;

typedef struct{
	//��ʼ��hlw
	void (*init)( void );

	//��ȡ��������
	int (*get_cyc_pul)( int road,void* data );
	
	//��ѯ��ҵ
	//
	void (*listen_work)( void );
	
}sock_hlw_opt_t,*sock_hlw_opt_pt;

/*----------------------------------------------------------------------------------*/
