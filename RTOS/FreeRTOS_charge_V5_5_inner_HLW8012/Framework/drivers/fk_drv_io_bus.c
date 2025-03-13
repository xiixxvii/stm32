#include "framework.h"


#define PRT_LOG(X...)	sysprt->dlog//dprintf(X)	

/*--------------------------------------------------------------------------------*/
/*
initʱ�������chip����2�黺��ռ䣬�ֱ���ibuf��obuf��

��������|    |  ���� |     |Ӳ������  | Ӳ������
[cslock]|<-- |ibuf[ ]| <-- |          |<--HC165
        |    |       |     | work()   | 
[ mq   ]|--> |obuf[ ]| --> |      ����|-->HC595
        |    |       |     |          |

ibuf[]����work()��app����Ϊ�����ϵ��
����ֻ��work()���޸�ibuf[]�����ֵ��
����ֻ��Ҫʹ��cslock����ɷ���ʵ�ֻ����ϵ��
ͬ����obuf[]����work()��app����Ϊ�����ϵ��
����work()ֻ��obuf[]�����ݣ�����app�����ж���������obuf[]��
���ö���ͬ����ʽ���������app��ִ��Ч�ʡ�

�����ṹ
fk_drv_io_bus.c
|
|-fk_drv_port_io_bus.c	
|
|-byte_array.c


���ļ������Ű�ṹ

(1)ibufָ���obufָ��
(2)ʵ����������ibuf�Ľṹ��bit_ctrl_t do_bit_ctrl_in
		�ڲ����� �� ��ʵ�ֻ�����ʡ� 
(3)ʵ����������obuf�Ľṹ��static bit_ctrl_t do_bit_ctrl_out
		�ڲ���������Ҳʵ��ͬ���������÷�����Ч����󻯡�
*/


/*--------------------------------------------------------------------------------*/

static uint8_t *ibuf = 0;
static uint8_t *t_ibuf = 0;
static uint8_t *obuf = 0;

/*--------------------------------------------------------------------------------*/
static thread_cslock_t cslock;
static void init_bit_ctrl_of_in(void)
{
	cslock = thread_cslock_init("bit_ctrl_of_in");	
}

static void in_set_bit( uint8_t ibit )
{
	//�����߼�����Ҫ�˽ӿ�
}
static void in_clr_bit( uint8_t ibit )
{
	//�����߼�����Ҫ�˽ӿ�
}
static int  in_get_bit( uint8_t ibit )
{
	uint8_t tbyte = 0;
	uint8_t chip = ibit/8;
	thread_cslock_lock(cslock,0XFFFFFFFF);
	
	tbyte = ibuf[ chip ];								
	thread_cslock_free(cslock);
	
	int val = ( tbyte >> (ibit&7) )&0x01;
	return val;
	
}


static bit_ctrl_t do_bit_ctrl_in={
	.set_bit = in_set_bit,
	.clr_bit = in_clr_bit,
	.get_bit = in_get_bit,
};
/*--------------------------------------------------------------------------------*/
typedef struct{
	uint8_t action;
	uint8_t ibit;
	uint8_t resver[2];
}apin_t,*apin_pt;
#define _action_clr  0
#define _action_set 1
static uint8_t mq_init_flag=0;				//���������������Ƿ񱻳�ʼ��

static thread_mq_t local_mq;
static void init_bit_ctrl_of_out(void)
{
	if( 1 == mq_init_flag ) return;
	local_mq = thread_mq_init("local_mq", sizeof(apin_t), 20);
	mq_init_flag = 1;
}

static void out_set_bit( uint8_t ibit )
{
	if( mq_init_flag == 0 ) return;
	apin_t apin;
	apin.action = _action_set;
	apin.ibit = ibit;
	
	thread_mq_send( local_mq,&apin, sizeof(apin_t), 0x01 );
	
}
static void out_clr_bit( uint8_t ibit )
{
	if( mq_init_flag == 0 ) return;
	apin_t apin;
	apin.action = _action_clr;
	apin.ibit = ibit;
	
	thread_mq_send( local_mq,&apin, sizeof(apin_t), 0x01 );
	
}

static int  out_get_bit( uint8_t ibit )
{
	uint8_t tbyte = 0;
	uint8_t chip = ibit/8;
	
	tbyte = obuf[ chip ];								
//	int val = (tbyte>>(ibit%8))&0x01;
	int val = (tbyte>>(ibit&(7)))&0x01;
	return val;							
	
}

static int read_mq_from_bit_ctrl( apin_pt pin )
{
	int ret = thread_mq_recv( local_mq, pin, sizeof(apin_t), 0);
	if(ret != bFALSE) 
		return 0;
	else
		return -1;
}

static bit_ctrl_t do_bit_ctrl_out={
	.set_bit = out_set_bit,
	.clr_bit = out_clr_bit,
	.get_bit = out_get_bit,
};

/*--------------------------------------------------------------------------------*/
static int chip_num = 0;
static int buf_size = 0;

//���ڿ��ٸ�������
static int buf_word_size = 0;
static uint32_t* src_copy  =0;
static uint32_t* dest_copy =0;

static void init( uint8_t chip )
{
	int i=0;
	
	init_bit_ctrl_of_in();
	init_bit_ctrl_of_out();
	//��ʼ��IO��
	drvp_io_bus->init();
	chip_num = chip;
	//��4����
	buf_size = (chip_num+0x03)&0xfffffffc ;
	PRT_LOG("\r\n");
	PRT_LOG("io_bus chip_num->%d / buf_size->%d \r\n",chip_num,buf_size);
	buf_word_size = buf_size/4;
	//Ϊbuf����ռ�
	ibuf = (uint8_t*)mem_alloc( buf_size );
	
	obuf = (uint8_t*)mem_alloc( buf_size );
	PRT_LOG("\tio_bus ibuf addr->0x%08x \r\n",(uint32_t)ibuf);
	PRT_LOG("\tio_bus obuf addr->0x%08x \r\n",(uint32_t)obuf);

	t_ibuf = (uint8_t*)mem_alloc( buf_size );			//�洢SPI ����������
	

	//��ǰ׼�����ַ���ָ�룬��SPI�������ݺ󣬿��ٸ��Ƶ�ibuf��
	src_copy  = (uint32_t*)t_ibuf;
	dest_copy = (uint32_t*)ibuf;
	
	
	memset( ibuf, 0xFF, buf_size );				//����� Ĭ��ȫ�Ǹߵ�ƽ
	memset( obuf, 0, buf_size );					//����� Ĭ��ȫ�ǵ͵�ƽ
	
	//��Ʊ���ϵ�Ҫ����
	byte_array->lset_bit( chip_num, obuf, 44 );
	byte_array->lset_bit( chip_num, obuf, 45 );
	byte_array->lset_bit( chip_num, obuf, 46 );
	byte_array->lset_bit( chip_num, obuf, 47 );
	
	
	
	//���ϵ磬�������͵�HC595����
	//���ݽ���
	for( i=0;i<buf_size;i++ )
	{	
		drvp_io_bus->swap( obuf[ buf_size - 1 - i ] );
	}
	
	drvp_io_bus->update();
}



static void work( void )
{
	int i = 0;
	apin_t apin;
	//ÿ��ץȡ���5������
	i=5;
	while(i--)
	{
		//�Ӷ��л�ȡ����
		if( read_mq_from_bit_ctrl( &apin ) >= 0 )
		{
			switch( apin.action )
			{
				case _action_clr: 
					byte_array->lclr_bit( chip_num, obuf, apin.ibit ); 
					PRT_LOG(" byte_array->lclr_bit( chip_num, obuf, %d ) \r\n",apin.ibit);
					break;
				case _action_set:
					byte_array->lset_bit( chip_num, obuf, apin.ibit ); 
					PRT_LOG(" byte_array->lset_bit( chip_num, obuf, %d ) \r\n",apin.ibit);

					break;
				default:
					break;
			}
//			for(i=0;i<buf_size;i++)
//				PRT_LOG("%02x ",obuf[ i ]);
//			PRT_LOG("\r\n");
		}
		else
		{
			break;
		}
	}
	//���ݽ���
	drvp_io_bus->load();
	for( i=0;i<buf_size;i++ )
	{	
		t_ibuf[i] = drvp_io_bus->swap( obuf[ buf_size - 1 - i ] );
	}
	//���ƶ��������ݵ�ibuf
	
	thread_cslock_lock(cslock,10);
	for( i=0;i<buf_word_size;i++ )
	{	
		dest_copy[i] = src_copy[i] ;
	}
	thread_cslock_free(cslock);
	
	drvp_io_bus->update();
}

static io_bus_t do_io_bus = {
	.init = init,
	.in = &do_bit_ctrl_in,
	.out = &do_bit_ctrl_out,
	.work = work,
};

io_bus_pt io_bus = &do_io_bus;


