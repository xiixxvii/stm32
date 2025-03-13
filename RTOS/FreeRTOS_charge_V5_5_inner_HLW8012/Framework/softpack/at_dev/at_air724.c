
#include "framework.h"
#define PRT_LOG(X...)	dprintf(X)	


at_dev_pt air724;



void init_air724(void)
{
	int ret = 0;
	int line = 0;
	int err = 0;
	char* action_name = NULL;
	
//------------------------------------------------------------------------------
//����AT�豸
	air724 = at_man->create_dev( uart1,20 );
	
//------------------------------------------------------------------------------
//�Զ�������,��Ƭ���̶���ATģ���Զ����
	action_name = "aoto_baudrate";
	ret = at_man->aoto_baudrate( air724,1000 );
	if( ret < 0 )
	{
		line = __LINE__;goto __AT_Init_ERROR;
	}
	task_delay(1000);
	while(1)
	{
		if( 0 == air724->uart->read_recv_buf( air724->rxbuf,1 ) )
		{
			break;
		}
		else
		{
			aprintf("%s\r\n",air724->rxbuf);
		}
	}
	
//------------------------------------------------------------------------------
//��Ϊ���ֵ�ʱ�����˺ܶ�"AT"�����������������۷�ʲô���Ƿ���+CME ERROR: 58
	err = 0;
__CMD_AT_1st:
	action_name = "__CMD_AT";
	ret = at_man->send_cmd( air724,"AT",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_1st;
	}
//------------------------------------------------------------------------------
//��ȡģ�鳧����Ϣ
	err = 0;
__CMD_AT_CGMI:
	action_name = "__CMD_AT_CGMI";
	ret = at_man->send_cmd( air724,"AT+CGMI",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CGMI;
	}
	
//------------------------------------------------------------------------------
//��ȡ�̼��汾��Ϣ	
	err = 0;
__CMD_AT_CGMR:
	action_name = "__CMD_AT_CGMR";
	ret = at_man->send_cmd( air724,"AT+CGMR",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CGMR;
	}
//------------------------------------------------------------------------------
//��ѯ���Ƿ���
	err = 0;
__CMD_AT_CPIN:
	action_name = "__CMD_AT_CPIN";
	ret = at_man->send_cmd( air724,"AT+CPIN?",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CPIN;
	}
//------------------------------------------------------------------------------
//��ѯ�����ź�����
	err = 0;
__CMD_AT_CSQ:
	action_name = "__CMD_AT_CSQ";
	ret = at_man->send_cmd( air724,"AT+CSQ",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CSQ;
	}
//------------------------------------------------------------------------------
//��ѯ����ע��״̬
	err = 0;
__CMD_AT_CREG:
	action_name = "__CMD_AT_CREG";
	ret = at_man->send_cmd( air724,"AT+CREG?",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CREG;
	}
	
	ret = at_man->send_cmd( air724,"\r\n",0XFFFFFFFF,2000 );
//------------------------------------------------------------------------------
#if 1
//��ѯ����GPRS����

	err = 0;
__CMD_AT_CGATT:
	action_name = "__CMD_AT_CGATT";
	ret = at_man->send_cmd( air724,"AT+CGATT?\r",0XFFFFFFFF,2000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		eaprintf("\r\n(%d) ret = %d \r\n",__LINE__,ret);
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CGATT;
	}
#endif
//------------------------------------------------------------------------------
//�����Զ���ȡAPN
	err = 0;
__CMD_AT_CSTT:
	action_name = "__CMD_AT_CSTT";
	ret = at_man->send_cmd( air724,"AT+CSTT=\"\",\"\",\"\"",0XFFFFFFFF,2000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CSTT;
	}

	
//------------------------------------------------------------------------------
//�����ƶ�������������ܻ�ȡ��IP
	err = 0;
__CMD_AT_CIICR:
	action_name = "__CMD_AT_CIICR";
	ret = at_man->send_cmd( air724,"AT+CIICR",100,1000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIICR;
	}

//------------------------------------------------------------------------------
//��ѯIP��ֻ�л�ȡ��IP���������
	err = 0;
__CMD_AT_CIFSR:
	action_name = "__CMD_AT_CIFSR";
	ret = at_man->send_cmd( air724,"AT+CIFSR",0xFFFFFFFF,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIFSR;
	}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//��͸���������ݷ���ģʽ-��ģʽ
	err = 0;
__CMD_AT_CIPQSEND:
	action_name = "__CMD_AT_CIPQSEND";
	ret = at_man->send_cmd( air724,"AT+CIPQSEND=1",1000,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIPQSEND;
	}	
//------------------------------------------------------------------------------
//���÷������ݳ���
	err = 0;
__CMD_AT_CIPSEND:
	action_name = "__CMD_AT_CIPSEND";
	ret = at_man->send_cmd( air724,"AT+CIPSEND=78",1000,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIPSEND;
	}
//------------------------------------------------------------------------------
//�������ݳ�
	err = 0;
__CMD_AT_SEND_DATA:
	action_name = "__CMD_AT_SEND_DATA";
	PRT_LOG("GET /skin2/save/fudai/riqi.php HTTP/1.1\nHOST: \nUser.Agent: \nConnection:Close\n\n");
	ret = at_man->send_cmd( air724,"GET /skin2/save/fudai/riqi.php HTTP/1.1\nHOST: \n\n",0XFFFFFFFF,5000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_SEND_DATA;
	}



	
//------------------------------------------------------------------------------
//�ر�����
	err = 0;
__CMD_AT_CIPCLOSE:
	action_name = "__CMD_AT_CIPCLOSE";
	ret = at_man->send_cmd( air724,"AT+CIPCLOSE=0",1000,3000 );
	at_man->printf_rxbuf( air724, NULL );
	if( ret < 0 )
	{
		
		if( ++err>=10 ) 
		{
			line = __LINE__;goto __AT_Init_ERROR;
		}
		goto __CMD_AT_CIPCLOSE;
	}
//------------------------------------------------------------------------------
	return;	
//------------------------------------------------------------------------------

//������
__AT_Init_ERROR:
	
	eaprintf("\r\n%s (%d) __AT_Init_ERROR: %s \r\n",__FILE__,line,action_name);
	
	return;
}

static void clr_recv_buf( at_dev_pt dev )
{
	uint8_t* rxbuf = dev->rxbuf;							//���ջ�����ָ�룬ָ��ᱻ�޸�
	//�Ȱѽ��ջ��������
	while( 1 )
	{
		if( 0 == dev->uart->read_recv_buf( rxbuf,1 ) )
		{
			//PRT_LOG("clr uart rxbuf\r\n");
			break;
		}
	}
}


//�����ַ���
static void send_str(at_dev_pt dev,char *string)
{
	uint32_t ptr=0;
	while ( string[ptr] != '\0' )
	{
		dev->uart->send( (uint8_t*)(&string[ptr]),1 );
		ptr++;
	}
}

/*

	����TCP����

*/
static int create_tcp( at_dev_pt dev )
{
	int err = 0,ret = 0,line = 0;
	
	uint32_t err_tick = 0;										//��������δ�
	uint32_t old_tick = 0,now_tick = 0;				//tick ��						
	uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len
	uint8_t* rxbuf = dev->rxbuf;							//���ջ�����ָ�룬ָ��ᱻ�޸�
	uint32_t cto = 3000;
	
	char * action_name = "__CMD_AT_CIPSTART";
	//����TCP����
	err = 0;
	//��ս��ջ�����
	clr_recv_buf( dev );
	//��������
	send_str( dev,"AT+CIPSTART=\"TCP\",\"14.116.153.234\",80" );
	//���ͻس�
	send_str( dev,"\r\n" );

__again:	
	//׼��������
	dev->rxlen = 0;
	err_tick =0;
	old_tick = get_sys_ticks();
	
	while(1)
	{
		//�����ݵ�dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );
		dev->rxlen += rd_len;
		
		//����û���յ��κ�����
		if( rd_len == 0  ) 
		{
			ret = strlib->findstr( (char*)rxbuf,"\r\n");			
			if( ret > 0 )
			{//˵���յ��˻س�����
				if( 0 < strlib->findstr( (char*)rxbuf,"OK" ) )
				{//˵���յ�OK
					PRT_LOG("TCP create success \r\n");
					return 1;
				}
				if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
				{//˵���յ�ERROR
					PRT_LOG("TCP create error(%d) \r\n",err);
					if( err<5 ) err++;
					else
						return 0;
				}	
			}
			
			now_tick = get_sys_ticks();
			if( cto < ( now_tick - old_tick ) )
			{//˵��������ճ�ʱ��
				return -2;
			}
			continue;
		}
		err_tick = 0;
	}
	
}


