#include "framework.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define PRT_LOG(X...)	sysprt->alog(X)	

/*-------------------------------------------------------------------------*/
static int regiser_one( cmd_util_pt cmd );



/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*��������Ҫ�ڴ˴�ע��,��������ʹ��*/
static void init(void)
{
	extern void register_base_cmd(void);

	extern void register_debug_cmd(void);
	
	/*ע��������*/
	register_base_cmd();
	/*��չ����*/
	register_debug_cmd();
	
}
/*-------------------------------------------------------------------------*/

static cmd_util_pt m_cmd_map_head = NULL;
/*-------------------------------------------------------------------------*/
static cmd_util_pt get_head(void)
{
	/*���ر�ͷָ��*/
	return m_cmd_map_head;
}

/*-------------------------------------------------------------------------*/
//��������
static int regiser_one( cmd_util_pt cmd )
{
	cmd_util_pt t_cmd_util;
	/*�ж������Ƿ�Ϊ��*/
	if ( m_cmd_map_head == NULL )
	{//������ֻ�б�ͷһ��
		m_cmd_map_head   = cmd;
		cmd->next = NULL;
	}
	else
	{//��������˱�ͷ,����������ӱ�ͷ��ʼ��β��
		t_cmd_util = m_cmd_map_head;
		/*�ҳ���β*/
		while ( t_cmd_util->next != NULL)
		{
			t_cmd_util = t_cmd_util->next;
		}
		/*����*/
		t_cmd_util->next	  = cmd;
		//�л����µ���β
		t_cmd_util = t_cmd_util->next;
		//����µ���β
		t_cmd_util->next = NULL;
	}
	return 0;
}
/*-------------------------------------------------------------------------*/
//��ȡ����һ������(������)
static cmd_util_pt get_by_val( int val )
{
	return NULL;
}
/*-------------------------------------------------------------------------*/
//��ȡ����һ������(������)
static cmd_util_pt get_by_name( char* name )
{
	cmd_util_pt t_cmd_util = m_cmd_map_head;
	//printf("you want to Get the Console : %s \r\n",name);
	while ( t_cmd_util )
	{
		/*�Ƚ�����*/
		if ( strlib->strcmpn( name,t_cmd_util->name,strlen(name)) )
		{
			
			//printf("ConsoleV() return %s \r\n",t_cmd_util->name);
			/*��ֹ��������"Te"ʱ,�ܹ�ִ��"Test"���������*/
			if(strlen(name) == strlen(t_cmd_util->name) )
				return t_cmd_util;
		}   
		//printf("ConsoleV() Dealing %s \r\n",t_cmd_util->name);
		t_cmd_util = t_cmd_util->next;
	}
	//printf("Error:ConsoleV can't find! \r\n");
	return NULL;
}

/*-------------------------------------------------------------------------*/
extern int console_work(void);
extern int boot_wait(void);
extern void quit_wait( void );
static cmdline_man_t do_cmdline_man={
	init,
	get_head,
	regiser_one,
	get_by_val,
	get_by_name,
	console_work,
	boot_wait,
	quit_wait,
};

cmdline_man_pt cmdline_man = &do_cmdline_man;
/*-------------------------------------------------------------------------*/
