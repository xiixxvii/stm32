#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

//��ȡģ�鳧����Ϣ
//���� 1  ˵�� ����ص�������ȷ
//���� 0  ˵������ص�����˵��AT�豸����
//���� -1 ˵���ܾ�û���յ�������
//���� -2 ˵�����ʱ��

//static char* str_enter = "\r\n";


static uint32_t dto = 2000;
static uint32_t cto = 2000;

int air724_cmd_AT_CGMI( void* airdev,int argc,char** argv )
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
	air_man->send_str( dev,"AT+CGMI\r\n");
	task_delay(50);
	
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
			ret = strlib->findstr( (char*)rxbuf,"\r\n");			
			if( ret > 0 )
			{//˵���յ��˻س�����
				if( 0 < strlib->findstr( (char*)rxbuf,"OK" ) )
				{//˵���յ�OK
					return 1;
				}
				if( 0 < strlib->findstr( (char*)rxbuf,"ERROR" ) )
				{//˵���յ�ERROR
					return 0;
				}	
			}
			
			if( ( dto == 0 )||( dto == 0XFFFFFFFF ) )
			{//˵��������Ϣ���ձȽ���
				task_delay(10);
			}
			else
			{//˵��������Ϣ���ձȽϿ�
				err_tick++;
				task_delay(1);
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


