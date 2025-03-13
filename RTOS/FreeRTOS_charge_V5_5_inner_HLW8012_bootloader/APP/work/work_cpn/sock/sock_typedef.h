
typedef struct{
	char ddh[20];	//������
	uint16_t num;	//����������һ���豸
	uint16_t cmd;	//������������
	uint32_t dat;	//��������
	
}order_t,*order_pt;


#define _event_curr 		0
#define _event_sock   	1
#define _event_net			2
#define _event_recore		3

typedef struct{
	char ddh[20];		//������ 16�ֽڶ����ţ�һ�ֽ�'\0'
	uint32_t type;	//�����¼�����	=0 ������=1 �����¼�
	
	uint16_t num;		//����������һ���豸
	uint16_t val;		//�����¼�ֵ
	
	uint32_t dat;		//������������
	double curr;		//��������ֵ����
}event_t,*event_pt;


typedef struct{
	void (*init)( void );
	void (*send_order)( char* ddh, int num, int cmd, uint32_t dat  );
	void (*put_event)( char* ddh,int type,int num, 
					int val, uint32_t dat,double curr  );
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
	
};

/*---------------------------------------------------------------*/
//hlw8032





//typedef struct{
//	uint32_t SOCK_CONNECT;	//�����Ƿ���豸
//	double SOCK_P;				//��Ч����
//	double SOCK_Vol;			//������ѹ
//	
//}sock_hlw_dat_t,*sock_hlw_dat_pt;



#define cmd_start0 	0x5A
#define cmd_len			24

typedef struct{
	//��ʼ��hlw
	void (*init)( void );
	int (*cat_connect)( int road );
	double (*cat_epwr)( int road );
	double (*cat_ecurr)( int road );
	
	double (*cat_evol)( int road );
	
	//��ѯ��ҵ
	void (*work)( void );

	
}sock_hlw_opt_t,*sock_hlw_opt_pt;



