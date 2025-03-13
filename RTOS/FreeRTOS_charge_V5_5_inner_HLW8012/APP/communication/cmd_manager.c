
#include "task_inc.h"
#include "framework.h"

#include "communication.h"


static cmd_t cmd_lib[ cmd_max_num ];

static void unknow(void* frame){}

static cmd_t unknow_cmd ={
	.num = 0,
	.usage = 0,
	.cCode = 0,
	.exec = unknow
};
	
	


//ע��һ������
static int reg( uint32_t cCode, cmd_exec_t exec )
{
	int i = 0;
	/*�ҵ��յ�λ��*/
	for( i=0; i<cmd_max_num; i++ )
	{
		if( cmd_lib[i].usage != _isEmpty ) continue;
		
		cmd_lib[i].usage = _isUsed;
		cmd_lib[i].cCode = cCode;
		cmd_lib[i].exec = exec;
		return 0;	
	}
	return -1;
}

//�����������ҵ������
static int get_num( uint32_t cCode )
{
	int i = 0;

	for( i=0; i<cmd_max_num; i++ )
	{
		if( cmd_lib[i].usage == _isEmpty ) continue;
		
		if( cCode == cmd_lib[i].cCode )
			return i;
	}
	return -1;
}

//ע��һ������ 
static int unreg( uint32_t cCode )
{
	
	//�ҵ�����������Ӧ�������
	int index = get_num( cCode );
	
	cmd_lib[index].usage = _isEmpty;
	cmd_lib[index].cCode = 0;
	cmd_lib[index].exec  = unknow;
	return index;
	
}

//�����������Ѱ����
static void* matching( int cCode )
{
	int index = get_num( cCode );
	if( index <0 ) return &unknow_cmd;
	cmd_pt cmd = &cmd_lib[ index ] ;
	return cmd;
}
static void deal( void* rframe )
{
	cmd_pt cmd;

	cframe_pt frame = (cframe_pt)rframe;
	cmd = matching( frame->cCode );
	if( cmd <0 ) return;
	//��������
	cmd->exec( frame );
}
/*---------------------------------------------------------*/

static cmd_man_t m_cmd_man={
	.reg = reg,
	.unreg = unreg,
	.deal = deal
};

cmd_man_pt cmd_man = &m_cmd_man;
/*---------------------------------------------------------*/
static int init_cmd_manager(void)
{
	int i = 0;
	APP_LogOUT("init_cmd_manager");
	for( i=0;i<cmd_max_num;i++ )
	{
		cmd_lib[i].num = i;
		cmd_lib[i].usage = _isEmpty;
		cmd_lib[i].cCode = 0;
		cmd_lib[i].exec  = unknow;
	}
	return 0;
}

INIT_PREV_EXPORT( init_cmd_manager );

