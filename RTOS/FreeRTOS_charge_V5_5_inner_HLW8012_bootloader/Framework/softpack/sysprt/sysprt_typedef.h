
/*========================================================================*/

#define __UseInExfile 
typedef void(*ConsoleAPP)(unsigned char argc, char *argv[]);								//

//һ����������ݽṹ
//typedef struct ConsoleAction {
//	char  *name;																							//��������
//	char	*Usage;																							//ʹ�÷���
//	char	*Text;																							//��ϸ����
//	void	(*ConsoleAPP)(unsigned char argc, char *argv[]);		//�������
//	struct 	ConsoleAction *ptNext;														//Next �ṹ
//}T_ConsoleAction,*PT_ConsoleAction;
/*�����б�������*/
//typedef struct ConsoleMan {
//	PT_ConsoleAction 	(*GetHead)(void);					
//	int (*Register)(PT_ConsoleAction ptConsoleAction);
//	PT_ConsoleAction (*GetByVal)(unsigned short pcVal);
//	PT_ConsoleAction (*GetByName)(char *name);
//}ConsoleMan_t;
//extern ConsoleMan_t ConsoleMan;
//extern void console_PrintCmdInfo(PT_ConsoleAction ptConsoleAction);
#define PrintHelp() {\
	PT_ConsoleAction ptConsoleAction = ConsoleMan.GetByName(argv[0]);	\
	console_PrintCmdInfo(ptConsoleAction);}


/*========================================================================*/

enum ptf_type{
	__type_stdlog__ = 0,		//��׼��־
	__type_drvlog__,				//������־
	__type_applog__,				//Ӧ����־
	__type_drverr__,				//��������
	__type_apperr__,				//Ӧ�ô���	
};

typedef struct{
	void (*init)( void );
	void (*work)( void );
	
	void (*log )( char* format,... );
	void (*dlog)( char* format,... );
	void (*alog)( char* format,... );
	void (*derr)( char* format,... );
	void (*aerr)( char* format,... );
	
	void (*tlog )( char* format,... );
	void (*tdlog)( char* format,... );
	void (*talog)( char* format,... );
	void (*tderr)( char* format,... );
	void (*taerr)( char* format,... );
	
	//���ô���
	void (*set_uart)( uart_pt uart );
	//���ش���
	uart_pt (*port)(void);
	
	
}sysprt_t,*sysprt_pt;

typedef struct _cmd_util{
	char  *name;																							//��������
	char	*usage;																							//ʹ�÷���
	char	*text;																							//��ϸ����
	int (*app)(char argc,char** argv);
//	struct _cmd_util* last;
	struct _cmd_util* next;
}cmd_util_t,*cmd_util_pt;

typedef struct{
	void (*init)(void);																				//��ʼ�����������
	cmd_util_pt (*get_head)( void );													//��ȡ��ͷ
	int (*regiser_one)( cmd_util_pt cmd );										//ע��һ������
	cmd_util_pt (*get_by_val)( int val );										//��������Ż�ȡһ������
	cmd_util_pt (*get_by_name)( char* name );									//�������ֻ�ȡһ������
	int (*work)(void);
	int (*boot_wait)(void);
	void (*quit_wait)( void );															//�˳��ȴ�
}cmdline_man_t,*cmdline_man_pt;








