
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

static void air724_init_hardware( wk_inet_pt inet );
//------------------------------------------------------------------------------

void work_internet_init_hardware( wk_inet_pt inet )
{
//	static time_t otick=0,ntick=0;
	switch( inet->ssta )
	{
		case 0:
			air724_init_hardware( inet );
		
			break;
		
		default:
			break;
	}
	
}
//------------------------------------------------------------------------------


#define CMD_MaxErr	3
#define __ERR_AIR_RST( ) 	{ line = __LINE__;goto __AIR_RST; }

static void air724_init_hardware( wk_inet_pt inet )
{
	
//	char* argv[5];

	int err = 0,ret = 0,line = 0;

		
	air_dev_pt  this_dev;
	this_dev = air724;
	
		
	//-----------------------------------------------------------------
	if( wdat->net_mode == NetMode_Air724 )
	{//Air724֧���Զ�ƥ�䲨����
		//ƥ�䲨����
		air724->auto_baudrate( this_dev,NULL,NULL );
		//ƥ���ʱ�򣬷����˺ܶ�AT���豸��Ҫ���� "AT\r\n" �������������
		while( 1 )
		{		
			ret = air724->cmd_AT( this_dev,NULL,NULL );
			air_man->ptf_rxbuf( this_dev,NULL );
			if( ret == 1 ) break;
		}
	}
	
	//��ȡģ�鳧����Ϣ
	air724->cmd_AT_CGMI( this_dev,NULL,NULL );
	air_man->ptf_rxbuf( this_dev,NULL );
	
	//��ȡ�̼��汾��Ϣ
	air724->cmd_AT_CGMR( this_dev,NULL,NULL );
	air_man->ptf_rxbuf( this_dev,NULL );
	
	//��ѯ���Ƿ���
	err = 0;
	while( 1 )
	{
		ret = air724->cmd_AT_CPIN( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(500);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//��ѯ�����ź�����
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CSQ( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(1000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//��ѯ����ע��״̬
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CREG( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//��ѯ����GPRS����
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CGATT( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	//����͸��ģʽ
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPMODE( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//����IPΪ������
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPMUX( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	//�����Զ���ȡAPN
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CSTT( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//�����ƶ�������������ܻ�ȡ��IP
	err = 0;
	while( 1 )
	{		 
		ret = air724->cmd_AT_CIICR( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>1 ){__ERR_AIR_RST();}
	}
	
	//��ѯIP��ֻ�л�ȡ��IP���������
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIFSR( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		task_delay(2000);
		if( (err++)>1 ){__ERR_AIR_RST();}
	}
	
	
	inet->msta++;
	inet->ssta = 0;
	return ;
//	air_man->send_str( this_dev,"+++" );
__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	//����λ
	inet->msta = 0;
	inet->ssta = 1;
	return;
}






