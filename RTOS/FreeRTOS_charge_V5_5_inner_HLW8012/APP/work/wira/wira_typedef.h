

typedef struct{
	void (*feed)( wk_inet_pt inet );
	int (*deal)( wk_inet_pt inet );
	
}nwdt_t,*nwdt_pt;

typedef struct{
	uint32_t code;
	uint32_t terminal;	//�ն˺�
	uint32_t order;			//����
	char ddh[20];				//��Ŷ����ţ���������Ϊֻ����16���ֽ�
	uint32_t sock;			//����������
	uint32_t time;			//��������ʣ����ʱ�� ��λ ms
	uint32_t utime;			//������������ʱ��
}rDat_t,*rDat_pt;


typedef struct{
	uint32_t socket;	//������
	uint32_t time;		//���ʱ��
}ddInfo_t,*ddInfo_pt;

//-------------------------------------------------------------------------
//������
typedef struct pool{
	struct pool *last;		//��һ��
	
	uint32_t sta;					//״̬��ֵ
	uint32_t num;					//��Ŀ��
	uint32_t dest_cntr;		//Ŀ�귢�ʹ���
	uint32_t now_cntr;		//��ǰ���ʹ���
	uint32_t wait_tick;		//�ȴ�ʱ��
	time_t otick;					//��һ�η��͵�ʱ��
	
	event_t event;				//��Ҫ���͵�����
	struct pool *next;		//��һ��
}pool_t,*pool_pt;

typedef struct{
	void (*init)( void );
	
	void (*register_one)( pool_pt pool );
	int (*delate_one)( pool_pt pool );
	
	pool_pt (*create)( uint32_t dest,uint32_t wait,event_pt event );
	void (*destroy)( pool_pt pool );
	
	pool_pt (*get_by_ddh)( char* ddh );
	
	void (*work)( void );
	
}ipool_t,*ipool_pt;
//-------------------------------------------------------------------------
//�����ö��һ��Ҫ���ַ��� ����Ե��ϣ�����͹ϲ�

enum{ 
	_srv_ret_logon = 0,				//���ص�¼��Ϣ
	_srv_ret_relogin,					//����relogin
	_srv_ret_nologin,					//����nologin
	_srv_ret_over,						//����over
	_srv_ret_log,							//����log ʵʱ�����ϱ�
	_srv_ret_pong,						//����pong
	_srv_ret_PARAM,						//�����ϱ��ն˲���
	_srv_ret_STATU,						//�����ϱ��ն�״̬
	_srv_ret_jfpg,						//���ؼ��ƽ��
	_srv_ret_time,						//����ʱ��
	_srv_cmd_setparam,				//���ò���
	_srv_cmd_getstatu,				//��ȡ״̬
	_srv_cmd_update,					//����OTA
	_srv_cmd_on,							//�͵�
	_srv_cmd_off,							//ͣ��
	_srv_cmd_reboot,					//��������, Э��û��, �ȱ�����������
	_srv_cmd_max,
};

//����ֵ����
#define ret_RstNet		-10			//��λ����
#define ret_ReConnect	-9			//��������

#define ret_Unknow	-1				//����ʧ�� δ֪
#define ret_Null		0					//��ͨ
#define ret_Success	1					//�ɹ�

typedef int (*deal_srv_cmd_t)( uint8_t* dat );
//-------------------------------------------------------------------------




//-------------------------------------------------------------------------

