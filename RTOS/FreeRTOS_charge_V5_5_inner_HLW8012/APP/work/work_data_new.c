#include "work.h"


#define PRT_LOG(X...)	sysprt->alog(X)	
//#define PRT_LOG(X...)	printf(X)	

/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------*/
//��̬��������

static boot_sta_t boot_sta ={0,0};	//����״̬
static total_e_t total_e;						//�����ܵ���
static rtd_num_t rtd_num;						//ʵʱ�������к�
static order_info_t order_info;			//����������Ϣ


static time_slot_t time_slot;				//ʱ���


wdat_guide_t m_wdat_guide = {
	.boot_sta = &boot_sta,
	.time_slot = &time_slot,
	.total_e = &total_e,
	.rtd_num = &rtd_num,
	.order_info = &order_info,
};

wdat_guide_t *guide_dat;

extern fdb_kvdb_t syskvdb;
extern int flashdb_boot_init(void);

#define FDB_BOOT_Flag (BOOT_CODE+0x5)
/*----------------------------------------------------------------------------------*/
void wdat_guide_init(void)
{
	int ret = 0;
	//����
	guide_dat = &m_wdat_guide;
	
	//����
	int result = flashdb_boot_init();
	PRT_LOG("\r\n result: [%s]!\r\n", (result == 0) ? "OK" : "ERROR");
	
	//------------------------------------------------------------------
	//����Ҫ�洢������
	//��ʱʱ��
	guide_dat->time_slot->get_date = 0;
	guide_dat->time_slot->get_slot = 0;
	guide_dat->time_slot->segments = 0;
	
	
	
	
	//------------------------------------------------------------------	
	//��ȡ������
	ret = wdat_opt->boot_sta.get_by_fdb(  );
	if( ret >= 0 )
	{
		if( guide_dat->boot_sta->code == FDB_BOOT_Flag )
		{//����, ˵���Ѿ���ʼ�������ݿ�
			PRT_LOG("fdb is not first boot ->%d\r\n",guide_dat->boot_sta->cntr);
			guide_dat->boot_sta->cntr++;
			
			wdat_opt->boot_sta.set_to_fdb( &boot_sta );
			
			//-----------------------------------------------------------
			//��ȡ���ݵ��ڴ�,ֻ�����ȡ��ʣ�µĽ���work����
			wdat_opt->total_e.get_by_fdb( );			//�ܵ���
			wdat_opt->rtd_num.get_by_fdb( );			//ʵʱ���
			wdat_opt->order_info.get_by_fdb( );		//������Ϣ
			
		}
		else
		{//����, ˵�����ݿ��ʼ����,�������ݿ�����ݲ��Ծ�
			PRT_LOG("fdb is first bootd\r\n");
			goto __fdb_init;
		}
		
	}
	else
	{//����
		PRT_LOG("ERR:wdat_opt->boot_sta.get_by_fdb \r\n");
		goto __fdb_init;
	}
	
	return;
__fdb_init:	
	guide_dat->boot_sta->code = FDB_BOOT_Flag;
	guide_dat->boot_sta->cntr = 0;
	
	wdat_opt->boot_sta.set_to_fdb( &boot_sta );

	//���ܳ�ʼ��
	guide_dat->total_e->pul[0] = 0;
	guide_dat->total_e->pul[1] = 0;
	wdat_opt->total_e.set_to_fdb( &total_e );
	
	//��ʱʱ�γ�ʼ��
//	memset( &time_slot, 0, sizeof( time_slot_t ) );
//	wdat_opt->time_slot.set_to_fdb( &time_slot ) ;
	
	//ʵʱ�������к�
	memset( &rtd_num, 0, sizeof( rtd_num_t ) );
	wdat_opt->rtd_num.set_to_fdb( &rtd_num ) ;
	
	//������Ϣ��ʼ��
	memset( &order_info, 0, sizeof( order_info_t ) );
	wdat_opt->order_info.set_to_fdb( &order_info ) ;
	
	
	
	
	
	return;

}
/*==================================================================================*/
//index ������һ������
//str 	ʱ�䴮
 void time_slot_dump(int group,char* str)
{
	uint32_t start = 0;
	uint32_t end = 0;
	uint32_t val = 0;
	int index = guide_dat->time_slot->segments;
	char buf[4];
	
	//���뿪ʼʱ��� ʱ
	for( int i=0;i<sizeof(buf);i++ ) buf[i] = 0 ;
	buf[0] = str[0];
	buf[1] = str[1];
	val = atol( buf );
	start = val * 60;
	
	//��ȡ��ʼʱ��� ��
	for( int i=0;i<sizeof(buf);i++ ) buf[i] = 0 ;
	buf[0] = str[3];
	buf[1] = str[4];
	val = atol( buf );
	start = start + val;
	
	//�������ʱ��� ʱ
	for( int i=0;i<sizeof(buf);i++ ) buf[i] = 0 ;
	buf[0] = str[6];
	buf[1] = str[7];
	val = atol( buf );
	end = val * 60;
	
	//��ȡ����ʱ��� ��
	for( int i=0;i<sizeof(buf);i++ ) buf[i] = 0 ;
	buf[0] = str[9];
	buf[1] = str[10];
	val = atol( buf );
	end = end + val;
	
	//���ʱ��
	guide_dat->time_slot->val[ index ].group = group;
	guide_dat->time_slot->val[ index ].start = start;
	guide_dat->time_slot->val[ index ].end = end;
	
	
	guide_dat->time_slot->segments++;
	
}

/*==================================================================================*/
// base ����
//��ȡ�������ݣ�ֱ��д�ڱ�����,��ʹ���������
static int get_by_fdb( fdb_kvdb_t kvdb,void* dat,uint32_t len,char* name )
{
	struct fdb_blob blob;
	fdb_kv_get_blob( kvdb, name, fdb_blob_make(&blob, dat, len ) );
	if (blob.saved.len > 0) {
		PRT_LOG("get the '%s' success\r\n", name );
		return 1;
	} else {
		PRT_LOG("get the '%s' failed\r\n",name);
		return -1;
	}
}

static int set_to_fdb( fdb_kvdb_t kvdb,void* dat,uint32_t len,char* name )
{
	int ret = 0;
	struct fdb_blob blob;
	ret = fdb_kv_set_blob( kvdb, name , fdb_blob_make( &blob, dat,len ) );
	if( ret ==  FDB_NO_ERR)
	{return 1;}
	else
	{return -1;}
}

/*==================================================================================*/
//������������
#define build_db_base_func( name ) \
	static int __get_##name##_by_fdb ( void )			{ return get_by_fdb( syskvdb,&name	,sizeof( name ),#name ); }\
	static int __set_##name##_by_fdb ( void* dat ){ return set_to_fdb( syskvdb,dat		,sizeof( name ),#name ); }

//����Ա����
#define dump_base_func_mb( name ) \
	.name 	={__get_##name##_by_fdb,__set_##name##_by_fdb,}
/*==================================================================================*/

	
	
build_db_base_func(boot_sta);
build_db_base_func(total_e);
build_db_base_func(rtd_num);
build_db_base_func(order_info);


/*==================================================================================*/

static wdat_opt_t do_wdat_opt = {
	dump_base_func_mb( boot_sta ),
	dump_base_func_mb( total_e ),
	dump_base_func_mb( rtd_num ),
	dump_base_func_mb( order_info ),

};

wdat_opt_pt wdat_opt = &do_wdat_opt;

