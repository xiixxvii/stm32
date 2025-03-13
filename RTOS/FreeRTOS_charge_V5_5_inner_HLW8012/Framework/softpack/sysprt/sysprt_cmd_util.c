#include "framework.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define PRT_LOG(X...)	sysprt->alog(X)	

extern cmdline_man_pt cmdline_man;

#define debug_uart uart0

#define MAX_ARG_NUM 				5							//���֧�������������
#define MAX_OneLineLen			512						//
#define PrintConsole_Args		0							//�Ƿ��ӡ�����������

static uint8_t line[MAX_OneLineLen];					//�л���
static uint8_t Cursor;							//��������̨��ǰ���λ��


/*-----------------------------------------------------------------------*/
#if 0
int split_string(char *str, char **result, char *delimiters) {
    int count = 0;
    char *token;

    // ʹ�� strtok ��������ַ���
    token = strtok(str, delimiters);

    while(token != NULL) {
        result[count] = token;
        count++;
        token = strtok(NULL, delimiters);
    }
		
    return count;
}
#endif
//int split_string(char *str, char **result, const char *delimiters) {
//    int count = 0;
//    char *token;
//    char *saveptr; // ���ڱ��������ĵ�ָ��

//    // ʹ�� strtok_r ��������ַ���
//    token = strtok_r(str, delimiters, &saveptr);

//    while (token != NULL) {
//        result[count] = token; // ���浱ǰ�� token
//        count++;
//        // ��ȡ��һ�� token
//        token = strtok_r(NULL, delimiters, &saveptr);
//    }

//    return count;
//}
/*-----------------------------------------------------------------------*/
//����0 ˵��û������ָ��
//���ط�0 ˵����argv[0]�ĳ���
static unsigned char process_one_line(char *cmd_line)
{
	cmd_util_pt t_cmd_util;
	unsigned char argc;
	static char * argv[MAX_ARG_NUM];
	char *pline;
	char *ret;
	
//	argc = strlib->split_string( cmd_line, argv," \t\n\r");
	argc = strlib->split_string_n( MAX_ARG_NUM,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;

	/*  ����ָ�����  */
	t_cmd_util = cmdline_man->get_by_name( argv[0] );
	if( t_cmd_util != NULL)
		t_cmd_util->app(argc,argv);
	return strlen(argv[0]);
}
/*-----------------------------------------------------------------------*/
int BootTime = 0;					//��������ʱ����
int EnCountdown = 0;	//=1 ʹ�ÿ�������ʱ,����ʱ�ڼ�,������س���,��ͣϵͳ����

static time_t otick_down=0,ntick_down = 0;
/*
//����ֵ
0	:˵��û���յ�ָ������
1	:˵���յ�ָ������
2	:˵���ȴ���ʱ,��������ϵͳ
3	:˵����ֹ�����ɹ�
*/


int console_work(void)
{
	unsigned char i;
	int ret = 0;
	uint8_t buf[16];
	
	static unsigned char ExKey = 0;					//��չ������־,����ʶ������Ȱ���	
	static unsigned char ExKeyState = 0;		//��չ����״̬
	
	if( 1 == EnCountdown ){
		/*�Ӵ��ڵĻ����������������*/
		ret = debug_uart->read_recv_buf( buf, sizeof(buf));
		if( ret <= 0 ) return 0;
		
		ret = strlib->findstr( (char *)buf,"0603\r\n" );
		if( ret >= 0 )
		{
			EnCountdown = 0;
			
		}
	}
	/*-------------------------------------------------------------*/
	/*�Ӵ��ڵĻ����������������*/
	ret = debug_uart->read_recv_buf( buf, sizeof(buf));
	if (ret <= 0) return 0;
	/*-------------------------------------------------------------*/
	/*������ڿɶ�����*/
	for (i = 0; i < ret; i++)
	{
		debug_uart->send( &buf[i], 1);						//���͵��ն�
		//PRT_LOG("\r\n								KeyVal = 0x%02X \r\n",buf[i]);
		/*--ʶ����չ��-----------------------------------*/
		if(ExKey)
		{
			if(ExKeyState==0){
				switch(buf[i])
				{
					/*--�����----------------*/
					case 0X5B:{
						ExKeyState = 1;
						break;
					}
					/*--ESC-------------------*/
					case 0X0D:{
						ExKeyState = 0;
						ExKey = 0;
						/*�µ�������*/
						Cursor = 0;
						PRT_LOG("$>");
						break;
					}
					default:{
						ExKeyState = 0;
					  ExKey = 0;
					}
				}
			}
			else{
				/*������ʲô��չ��*/
				switch(buf[i])
				{
					/*--�Ϸ����-------------------------------*/
					case 0X41:{//PRT_LOG("Up!\r\n");

						break;
					}
					/*--�·����-------------------------------*/
					case 0X42:{//PRT_LOG("Dn!\r\n");
						
						break;
					}
					/*--�ҷ����-------------------------------*/
					case 0X43:{//PRT_LOG("Right!\r\n");
						
						break;
					}
					/*--�����-------------------------------*/
					case 0X44:{//PRT_LOG("Left!\r\n");
						
						break;
					}
					default:{
						break;
					}
				}
				//�˳���չ����ģʽ
				ExKeyState = 0;
				ExKey = 0;
			}
		}
		/*--��ͨ��---------------------------------------*/
		else
		{
			switch(buf[i])
			{
				/*--�˸��-----------------------------------*/
				case 0x7F:
				case '\b':{
						if(Cursor>0) Cursor--;
						line[Cursor] = '\0';
						PRT_LOG("\r\n$>%s",line);
					break;
				}
				/*--TAB��------------------------------------*/
				case 0X09:{
					//��ȫ���빦��
					line[Cursor] = '\0';						/*�����������룬����겻�ƶ�*/
					//PRT_LOG("\r\n TAB:You input is %s",line);
					cmd_util_pt t_cmd_util = cmdline_man->get_head();
					uint8_t TAB_item = 0;			/*TAB�������������*/
					uint8_t TAB_item_buf[64];	/*���ڴ洢TAB���ĵ�һ��*/
					int iLen = 0;
					PRT_LOG("\r\n ");
					
					/*��������,���Ƿ��а��������ַ��������*/
					while( t_cmd_util != NULL)
					{
						if( strlib->strcmpn( (char*)line,t_cmd_util->name,Cursor) ){
							PRT_LOG("\t%s \r\n",t_cmd_util->name);										
							TAB_item ++;
							/*������ں�ѡ������Ƶ�һ������*/
							if(TAB_item==1) {
								for(i=0;i<64;i++) TAB_item_buf[i] = 0;
								iLen = strlen( t_cmd_util->name);
								memcpy(TAB_item_buf,t_cmd_util->name,iLen);
							}
						}
						t_cmd_util = t_cmd_util->next;
					}
					
					if(TAB_item == 0) {					/*��û�ҵ���������ʾһ��*/
						PRT_LOG("No list\r\n");
					}
					else if(TAB_item==1) {			/*��ֻ��һ��Զ���������*/
						iLen = strlen((char *)TAB_item_buf);
						memset(line,sizeof(line),'\0');
						memcpy(line,TAB_item_buf,iLen);
						Cursor = iLen;
						line[Cursor] = 0;
						PRT_LOG("\r\n$>%s",line);
					}else{											/*���ж����ѡ��,����*/
						PRT_LOG("\r\n$>%s",line);
					}
					break;
				}
				/*--��չ����---------------------------------*/
				case 0X1B:{
					ExKey = 1;
					ExKeyState = 0;
				}
				/*--Crel+Z-----------------------------------*/
				case 0X1A:{
					//�ڴ˴���ӳ�������
					break;
				}
				/*--Crel+C-----------------------------------*/
				case 0X03:{
					//�ڴ˴��������������
					
					break;
				}
				/*--�س���-----------------------------------*/
				case 0x0D:{
					/*  �������������  */
					line[Cursor] = '\0';
					PRT_LOG("\r\n");
					/*  ����һ������������  */
					if(Cursor>0)	process_one_line( (char*)line );
					//PRT_LOG("Len = %d \r\n",LenArgv0);
					/*  �������ݵ��л���,���ڻָ���һ������  */
					/*  �µ�������  */
					
					Cursor = 0;
					//for(i=0;i<MAX_OneLineLen;i++)	{line[i] = 0;}
					
					PRT_LOG("$>");
					break;
				}
				
				/*--�ǿ��Ƽ�---------------------------------*/
				default:{
					/*�������ݳ����л���*/
					if( Cursor >= sizeof(line) ) {
						PRT_LOG("\r\n Input data is too long!");
						PRT_LOG("Cursor = %d \r\n",Cursor);
						return 1;
					}else{
						line[Cursor] = buf[i];
						Cursor++;
					}
					break;
				}
			}
			/*--END_switch---------------------------------*/
		}
	}
	return 1;
}

int boot_wait(void)
{
	int ret = 0;
	uint8_t buf[16];
	while(1)
	{
		//	/*-------------------------------------------------------------*/
//	//����ʱ����
		if( 0 == EnCountdown ) break;
		
		ntick_down =  get_sys_ticks();
			
		if( 1000 > ( ntick_down - otick_down ) ) continue;
		
		otick_down = ntick_down;
		
		PRT_LOG("-> = %d \r\n",BootTime);
		
		if(BootTime>0)	{BootTime--;}
		else			
		{
			EnCountdown = 0;
			break;
		}
		
		
	}
	return 1;
}

