#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

//����TCP/UDP����
//���� 1  ˵�� ����ص�������ȷ
//���� 0  ˵������ص�����˵��AT�豸����
//���� -1 ˵���ܾ�û���յ�������
//���� -2 ˵�����ʱ��

//static char* str_enter = "\r\n";


static uint32_t dto = 4000;
static uint32_t cto = 4000;
//	argv[0] = 	TCP/UDP
//	argv[1] = 	IP addr
//	argv[2] = 	port

int air724_cmd_AT_CIPSTART( void* airdev,int argc,char** argv )
{
	air_dev_pt dev = airdev;

	int ret = 0;
	uint32_t err_tick = 0;										//��������δ�
	uint32_t old_tick = 0,now_tick = 0;				//tick ��										
	
	uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len
	uint8_t* rxbuf = dev->rxbuf;							//���ջ�����ָ�룬ָ��ᱻ�޸�
	
	//�Ȱѽ��ջ��������
	memset( rxbuf,0,1024 );
	air_man->clr_rxbuf( dev );
	//��������
	PRT_LOG("argv[0]:%s \r\n",argv[0]);
	PRT_LOG("argv[1]:%s \r\n",argv[1]);
	PRT_LOG("argv[2]:%s \r\n",argv[2]);
	
	//AT+CIPSTART="TCP","14.116.153.234",80\r
	
	//AT+CIPSTART=
	air_man->send_str( dev,"AT+CIPSTART=");
	//"TCP" of "UDP"
	air_man->send_str( dev,"\"");
	air_man->send_str( dev,argv[0] );	//TCP or UDP
	air_man->send_str( dev,"\"");
	//,
	air_man->send_str( dev,",");
	//"000.000.000.000"
	air_man->send_str( dev,"\"");
	air_man->send_str( dev,argv[1] );
	air_man->send_str( dev,"\"");
	//,
	air_man->send_str( dev,",");
	//port
	air_man->send_str( dev,argv[2] );
	
	//enter
	air_man->send_str( dev,"\r\n" );
	
		
	thread_delay_ms(150);
	
	//��ʼ������
	
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
			if( 0 < strlib->findstr( (char*)rxbuf,"CONNECT" ) )
			{//˵����������
				return 1;
			}
			
			ret = strlib->findstr( (char*)rxbuf,"\r\n");			
			if( ret > 0 )
			{//˵���յ��˻س�����
				
				if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
				{//˵���յ�ERROR
					return 0;
				}	
			}
			
			if( ( dto == 0 )||( dto == 0XFFFFFFFF ) )
			{//˵��������Ϣ���ձȽ���
				thread_delay_ms(10);
			}
			else
			{//˵��������Ϣ���ձȽϿ�
				err_tick++;
				thread_delay_ms(1);
				if( err_tick > dto )
				{//˵�����ݽ��ճ�ʱ��
					return -1;
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


