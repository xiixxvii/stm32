#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	//dprintf(X)	

static int auto_baudrate_one_cycle( air_dev_pt dev )
{
	uint8_t str[2] = {'A','T'};
	uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len
	uint8_t* rxbuf = dev->rxbuf;							//���ջ�����ָ�룬ָ��ᱻ�޸�

	//�Ȱѽ��ջ��������
	memset( rxbuf,0,1024 );
	air_man->clr_rxbuf( dev );
	//��������
	dev->uart->send( str,2 );
	task_delay(50);
	
	//��ʼ������
	dev->rxlen = 0;															
	while(1)
	{
		//�����ݵ�dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );
		dev->rxlen += rd_len;
		
		//����û���յ��κ�����
		if( rd_len == 0  ) 
		{
			
			if( ('A' == rxbuf[0]) && ( 'T' == rxbuf[1] )  )
			{
				
				return 0;
			}
			return -1;
		}
		
		
	}
}

int air724_auto_baudrate( void* airdev,int argc,void** argv )
{
	air_dev_pt dev = airdev;
	int ret = 0;
	uint8_t cntr = 0;
	while(1)
	{
		ret = auto_baudrate_one_cycle( dev );
		
		air_man->ptf_rxbuf( dev,NULL );
		
		if( ret >= 0 )
		{//˵���������
			PRT_LOG("aoto_baudrate success \r\n");
			return 0;
		}
		cntr++;
		if( cntr>10 ) 
		{
			PRT_LOG("aoto_baudrate faild \r\n");
			return -1;
		}
	}
}










