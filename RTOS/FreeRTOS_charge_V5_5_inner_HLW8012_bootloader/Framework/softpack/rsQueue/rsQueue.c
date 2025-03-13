

#include "framework.h"
#if 0
const int kAlign4 = 4; // kAlign show be powers of 2, say 2, 4 ,8, 16, 32, ...
const int kAlign8 = 8; // kAlign show be powers of 2, say 2, 4 ,8, 16, 32, ...
const int kAlign16 = 16;

int round_up4(unsigned int nBytes) { return ((nBytes)+(kAlign4 - 1)) & ~(kAlign4 - 1); }
int round_up8(unsigned int nBytes) { return ((nBytes)+(kAlign8 - 1)) & ~(kAlign8 - 1); }
int round_up16(unsigned int nBytes) { return ((nBytes)+(kAlign16 - 1)) & ~(kAlign16 - 1); }
#endif


static int upward_rounded_to_power_of_two( int val )
{
	int ret = 2;
	for( int i=0;i<16;i++ )//�˴����������ֵΪ32768
	{
		if( val > ret )
			ret <<=1;
		else
			break;
	}
	return ret;
}

static rsQCB_pt create( int size, int max,char* name )
{
	//����һ��rsQCB
	rsQCB_pt rsQCB = mem_alloc( sizeof( rsQCB_t ) );
	if( 0 == rsQCB )
	{
		APP_LogOut_ERROR("Error: mem_alloc  rsQCB (%dB) \r\n",sizeof(rsQCB_t ) );
		return 0;
	}
	//����ȡ����2����(Ŀ���Ǽ��ٺ�������������)
	//��Ϊ�����ڲ�����bug�����Է��֣�
	//������maxΪ16��ʵ����ֻ��write 15��Ԫ��
	//����ʹ��max+1����ʱ��װ�޸�bug
//	rsQCB->max = upward_rounded_to_power_of_two( max );
	rsQCB->max = upward_rounded_to_power_of_two( max+1 );
	
	//ΪrsQCB->array�������ռ䣬����Ŀռ��������ģ�Ԫ���Ƿ�Ϊ����ģ��˴�����ҪΪʹ���߲��ġ�
	rsQCB->array = mem_alloc( size * rsQCB->max );
	if( 0 == rsQCB->array )
	{
		APP_LogOut_ERROR("Error: mem_alloc  rsQCB->array (%dB)\r\n",size * rsQCB->max );
		return 0;
	}
	rsQCB->size = size;
	rsQCB->head = rsQCB->rear = 0;	
	aprintf("rsQCB [%s] create success \r\n",name);
	return rsQCB;
}

static int write( rsQCB_pt QCB, void* dat )
{
	uint8_t* src = dat;
	uint8_t* dest = (uint8_t*)( &(QCB->array[ QCB->head ]) );
	
	if( ( QCB->head+1 ) % QCB->max == QCB->rear )
	{
		/*TODO: Add error handling here.*/
		//debug_Assert(0,DPFL,"UART0:buffer full.");
		return -1;
	}
	else
	{
		//��Ϊ��֪�����ݵ����ͣ�����ֻ��ʹ�ø������ݵķ�ʽ���и�ֵ
		for( int i=0;i<QCB->size;i++ )
			dest[i] = src[i];
		QCB->head++;
		if(QCB->head >= QCB->max)
			QCB->head = 0;
	}
	
	return 0;
	
	
	
}

int read( rsQCB_pt QCB,void* dat )
{
	uint8_t* src = (uint8_t*)( &(QCB->array[ QCB->rear ]) );
	uint8_t* dest = dat;	
	
	
	if(QCB->rear == QCB->head)
	{
		return -1;
	}		
	//��Ϊ��֪�����ݵ����ͣ�����ֻ��ʹ�ø������ݵķ�ʽ���и�ֵ
	for( int i=0;i<QCB->size;i++ )
		dest[i] = src[i];
	
	QCB->rear ++;
	if(QCB->rear >= QCB->max)
		QCB->rear = 0;
	
	return 0;
}

static rsQueue_t do_rsQueue={
	.create = create,
	.write = write,
	.read =read,
};

rsQueue_pt rsQueue = &do_rsQueue;




