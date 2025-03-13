#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)		


static uint8_t* rxbuf = NULL;
static uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len




void wira_work_data_rst( void )
{
	//���ջ�������λ
			
	rxbuf = air724->rxbuf;
	memset( rxbuf,0,1024 );
	
	air724->rxlen = 0;
	
}

int wira_wrok_cycle( wk_inet_pt inet )
{
	int err = 0;
	int ret = 0;
	int cmd_ret = 0;
	//��������
	rd_len = air724->uart->read_recv_buf( &(rxbuf[ air724->rxlen ]),1 );
	//���û�У���ֱ���˳�
	if( rd_len <= 0 ) return -1 ;
	
	air724->rxlen += rd_len;

	//������ȳ���512,��������,���½���
	if( air724->rxlen >= 512 )
	{
		goto __invalid_data;
	}
	
	ret = strlib->findstr( (char*)rxbuf,"\r\n" );
	
	
	//���û���յ��س����ţ�ֱ���˳�
	if( ret < 0 ) return -2;
	
	
	
	//������ʲô����
/*
	�������ʹ�����
	(1)ATָ�����ݣ���������ۣ�����ҲҪ��λ���գ�������
	(2)Ӧ��Э�����ݣ���Э���ĵ�����
	(3)��Ч���ݣ�ֱ�Ӹ�λ���գ�������0
	
*/
	air_man->ptf_rxbuf( air724,NULL );
	
	//(1)ATģ�����Ϣ
	ret = strlib->findstr( (char*)rxbuf,"CLOSED" );
	if( ret >= 0 ) 
	{
		PRT_LOG( "recv CLOSED \r\n" );
		cmd_ret = ret_ReConnect;
		goto __at_cmd_data;
	}
	ret = strlib->findstr( (char*)rxbuf,"PING" );
	if( ret >= 0 ) 
	{
		PRT_LOG( "recv PING \r\n" );

		PRT_LOG( "PING ,is error \r\n" );
		cmd_ret = ret_RstNet;

		goto __at_cmd_data;
	}
	
	//(2)���������������������
	for( int index=0;index<_srv_cmd_max;index++ )
	{
		ret = strlib->findstr( (char*)rxbuf, _srv_cmd_list[ index ] );
		if( ret >= 0 ) 
		{
			//ִ�ж�Ӧ�ص�
			cmd_ret = wira_deal_cmd[index]( rxbuf );
			
			if( ret_RstNet == cmd_ret )
			{//��������
				goto __at_cmd_data;
			}
			else if( ret_ReConnect == cmd_ret )
			{//��������
				goto __at_cmd_data;
			}
			goto _protocol_data;
		}
	
	}
	
	goto __invalid_data;
	
__invalid_data:
	memset( rxbuf,0,1024 );	
	rxbuf = air724->rxbuf;
	air724->rxlen = 0;
	
	return 0;
__at_cmd_data:
	memset( rxbuf,0,1024 );	
	rxbuf = air724->rxbuf;
	air724->rxlen = 0;

	//�˳�͸��ģʽ
	thread_delay_ms( 1500 );
	air_man->send_str( air724,"+++" );
	thread_delay_ms( 1000 );
	
	//�ر�TCP����
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPCLOSE( air724,NULL,NULL );
		air_man->ptf_rxbuf( air724,NULL );
		if( ret >= 0 ) break;
		inet->ssta = 1;		
	}
	switch( cmd_ret )
	{
		case ret_ReConnect:
			PRT_LOG( "ret_ReConnect \r\n" );
			//�������ӷ�����
			inet->msta = 3;
			inet->ssta = 0;
			break;
		
		default:
			PRT_LOG( "ret_RstNet \r\n" );
			//��������ģ��
			inet->msta = 0;
			inet->ssta = 1;
			break;
	}
	
	return -3;
	

_protocol_data:
	nwdt->feed( inet ); //ι��
	
	
	memset( rxbuf,0,1024 );
	rxbuf = air724->rxbuf;
	air724->rxlen = 0;
	return 1;
	
}

