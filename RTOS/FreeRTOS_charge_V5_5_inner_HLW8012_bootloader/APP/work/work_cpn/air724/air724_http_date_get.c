#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)		


static uint8_t http_date[20];

char* get_date_str(void)
{
	return (char*)http_date;
}


static uint32_t dto = 4000;
static uint32_t cto = 4000;
extern char *http_serv;
int air724_http_date_get( void* airdev )
{
	air_dev_pt dev = airdev;

//	int enter_cntr = 0;												//�����յ��˶��ٸ��س�����
	int offset[4];
	
	int ret = 0,i = 0;
	uint32_t err_tick = 0;										//��������δ�
	uint32_t old_tick = 0,now_tick = 0;				//tick ��										
	
	uint32_t rd_len = 0;											//������ʱ���������ݳߴ� read len
	uint8_t* rxbuf = dev->rxbuf;							//���ջ�����ָ�룬ָ��ᱻ�޸�
	uint32_t err_cntr = 0;										//������
//	char* sub_str = NULL;
	
	//�Ȱѽ��ջ��������
	memset( rxbuf,0,1024 );
	air_man->clr_rxbuf( dev );
	//����http����
	PRT_LOG("http_req...\r\n");	
	
	air_man->send_str( dev,"GET /skin2/save/fudai/riqi.php HTTP/1.1\n" );
	

	air_man->send_str( dev,"HOST: \n" );
	
	
	air_man->send_str( dev,"Connection:Close\n" );
	air_man->send_str( dev,"\n" );
	air_man->send_str( dev,"\n" );
	
		
	thread_delay_ms(300);
	
	//��ʼ������
	dev->rxlen = 0;
	err_tick =0;
	old_tick = get_sys_ticks();
	err_cntr = 0;
	
	while(1)
	{
		//�����ݵ�dev->rxbuf
		rd_len = dev->uart->read_recv_buf( &(rxbuf[ dev->rxlen ]),1 );

		dev->rxlen += rd_len;
		
		//����û���յ��κ�����
		if( rd_len == 0  ) 
		{
			ret = strlib->get_substr_index( (char*)rxbuf,"\r\n\r\n", offset,4 );
			
			if( ret >= 2 )
			{//˵���յ���˫�س�����
				if( strlib->findstr( (char*)rxbuf,"HTTP/1.1 200 OK" ) < 0 )
				{//˵���յ����������������
					PRT_LOG("recv data is error \r\n");
					return -2;
				}
				PRT_LOG("enter x2 \r\n");
				PRT_LOG("offset[0] = %d \r\n",offset[0] );
				PRT_LOG("offset[1] = %d \r\n",offset[1] );
								
				//�������ַ������Ƴ���
				for( i=0; i<8; i++ )
				{
					http_date[ i ] = rxbuf[ offset[0] + 4 + 3 + i ];
				}
				http_date[i] = '\0';
				PRT_LOG("http_date:%s\r\n", http_date );
				
				return 1;
			}
			else
			{//˵����û�յ���Ҫ������
				if( strlib->findstr( (char*)rxbuf,"CONNECT" ) < 0 )
				{//˵�������У���û���ϣ�Ҫ��һ��
					err_cntr++;
					thread_delay_ms(10);
					if( err_cntr > 3 )
					{//˵��������ճ�ʱ��						
						return -1;
					}		
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



//static void get_date_to_rtc( void* rtc,uint8_t* dat  )
//{
//	
//	
//}

