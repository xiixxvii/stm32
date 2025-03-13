#include "framework.h"
#define PRT_LOG(X...)	dprintf(X)	



/*---------------------------------------------------------------------------------*/

//����һ��AT�豸
static at_dev_pt create_dev( uart_pt uart,uint32_t max_cmd )
{
	//�����ڴ���Ϊ�豸�ṹ
	at_dev_pt at_dev = mem_alloc( sizeof( at_dev_t ) );
	
	//�鿴�����Ƿ�ɹ�
	if( 0 == at_dev  )
	{	
		PRT_LOG("err:create_dev()-> mem_alloc( sizeof( at_dev_t ) )\r\n");
	}
	
	//���ô���
	at_dev->uart = uart;
	
	//������ջ�����
	at_dev->rxbuf = mem_alloc( sizeof( uint8_t ) * 4096 );
	//�鿴�����Ƿ�ɹ�
	if( 0 == at_dev->rxbuf  )
	{
		PRT_LOG("err:create_dev()-> mem_alloc(sizeof( uint8_t ) * 4096 ) \r\n");
	}
	
	
	
	return at_dev;
}

/*---------------------------------------------------------------------------------*/

//����һ��AT�豸
static void delete_dev( at_dev_pt dev	)
{
	
	mem_free( dev );
	
}
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
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

//hex 
//=0/NULL ˵����Ĭ�ϸ�ʽ�ַ���ʽ
//
static void printf_rxbuf( at_dev_pt dev , int hex )
{
	uint32_t max = dev->rxlen;
	if( max >= 4096 )
	{
		PRT_LOG("err: printf_rxbuf,len =%08d \r\n",dev->rxlen );
		return;
	}
	PRT_LOG("---------------\r\n")
	if( hex )
	{
		
		for( int i=0;i<max;i++ )
		{
			PRT_LOG("0x%02x ",dev->rxbuf[i])
		}
	}
	else
	{
		for( int i=0;i<max;i++ )
		{
			PRT_LOG("%c",dev->rxbuf[i])
		}
	}
	
}
static int auto_baudrate_one_cycle( at_dev_pt dev )
{
	uint8_t str[2] = {'A','T'};
	uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len
	uint8_t* rxbuf = dev->rxbuf;							//���ջ�����ָ�룬ָ��ᱻ�޸�

	//�Ȱѽ��ջ��������
	while( 1 )
	{
		if( 0 == dev->uart->read_recv_buf( rxbuf,1 ) )
		{
//			PRT_LOG("clr uart rxbuf\r\n");
			break;
		}
	}
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
static int aoto_baudrate( at_dev_pt dev,uint32_t cto )
{
	int ret = 0;
	uint8_t cntr = 0;
	while(1)
	{
		ret = auto_baudrate_one_cycle( dev );
		printf_rxbuf( dev,NULL );
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

//AT
//���� 1  ˵�� ����ص�������ȷ
//���� 0  ˵������ص�����˵��AT�豸����
//���� -1 ˵���ܾ�û���յ�������
//���� -2 ˵�����ʱ��

//static char* str_enter = "\r\n";

static int send_cmd( at_dev_pt dev,char* at,uint32_t dto,uint32_t cto )
{
	int ret = 0;
	uint32_t err_tick = 0;										//��������δ�
	uint32_t old_tick = 0,now_tick = 0;				//tick ��										
	
	uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len
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
	//��������
	send_str( dev,at );
	//���ͻس�
	send_str( dev,"\r\n" );
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
			
			if( '>' == rxbuf[dev->rxlen-1] ) return 3;//�յ�͸������
			
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



/*--------------------------------------------------------------------------------*/

static at_man_t do_at_man={ create_dev,delete_dev,aoto_baudrate,send_cmd,printf_rxbuf };

at_man_pt at_man = &do_at_man;






