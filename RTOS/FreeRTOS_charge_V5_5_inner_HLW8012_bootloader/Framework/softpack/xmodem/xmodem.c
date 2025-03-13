/*
 * Copyright        : Shendong MCU studio
 * File Description : xmodem protocol implementation
 * Created          : oliverzhang@shendongmcu.com
 */
#include "framework.h"
#include <stdlib.h>

#define Storge_Write(addr,buf,len) 	write_flash( buf,len,addr )	//SPI_Flash.EaserWrite(addr,buf,len) 
#define Storge_Read(buf,len,addr)  	read_flash(buf,len,addr)		//SPI_Flash.Read(buf,len,addr)
#define Storge_EaserChip()					//SPI_Flash.EaserChip()//	
/*---------------------------------------------------------------------*/

static void read_flash( uint8_t* buf,uint32_t len, uint32_t addr );

static void erase_flash_4k( uint32_t addr );

static void write_flash( uint8_t *buf,uint32_t len,uint32_t addr );
/*---------------------------------------------------------------------*/

//fw_WriteFlash
static unsigned char   g_xmodem_crc_mode = 1;

enum{CHAR_SOH=0x01,CHAR_STX=0X02,CHAR_EOT=0x04,CHAR_ACK=0x06,CHAR_NAK=0x15,CHAR_CAN=0x18,CHAR_C=0x43,};

/*CCITT: 0x11021: X^16 + X^12 + X^5 + 1, implemented nibble by nibble version*/
unsigned short crc_CalcCCITT16NibbleVer(void *vptr, unsigned short len) 
{
	unsigned char *ptr = (unsigned char *)vptr;
	unsigned short crc;
	unsigned char da;
	const unsigned short crc_ta[16]= {
		0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
		0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	};
	crc=0;
	while(len--!=0) 
	{
		da=((unsigned char)(crc/256))/16;
		crc<<=4;
		crc^=crc_ta[da^(*ptr/16)];
		da=((unsigned char)(crc/256))/16; 
		crc<<=4; 
		crc^=crc_ta[da^(*ptr & 0x0f)]; 
		ptr++;
	}
	return(crc);
}


static unsigned char PackIsValid(unsigned char *buf,unsigned char packno)
{
	unsigned char i,sum;
	unsigned char status = 0;						//����ʹ��,��������Э��״̬
	unsigned char crclow, crchigh;
	unsigned short crc;

	//uart2_Printf("~~~~~~~~~~~~~~~~packno = %bu\r\n",packno);
	if(buf[0] != CHAR_SOH) 
	{
		status = 1;
		goto errexit;
	}
	if(packno != buf[1])
	{
		status = 2;
		//uart2_Printf("PackFIALED: buf[1]=%bu,packno=%bu\r\n",buf[1],packno);
		goto errexit;
	}
	if( ((buf[1] + buf[2])&0xff) != 0xff)
	{
		status = 3;
		//uart2_Printf("PackFIALED: buf[1]=0x%bx,buf[2]=0x%bx\r\n",buf[1],buf[2]);
		goto errexit;
	}

	if(g_xmodem_crc_mode == 0)
	{
		for(i=0,sum=0;i<128;i++)
		{
			sum += buf[3+i];
		}
		if(sum != buf[131])
		{
			status = 4;
			//uart2_Printf("PackFIALED: sum=%bu,buf[131]=bu\r\n",sum,buf[131]);
			goto errexit;
		}
			//uart2_Printf("package OK,packno = %bu buf[1]=%bu\r\n",packno,buf[1]);
		return 1;
	}
	else
	{
		crc = crc_CalcCCITT16NibbleVer(&buf[3],128);
		crchigh = crc>>8;
		crclow = crc;
		if(crchigh == buf[131] && crclow == buf[132])
		{
			//uart2_Printf("package OK,packno = %bu buf[1]=%bu\r\n",packno,buf[1]);
			return 1;
		}
		else
		{
			status = 5;
			
			//uart2_Printf("PackFIALED: crc=0x%x,buf[131]=0xbx,buf[132]=0xbx\r\n",sum,buf[131],buf[131]);
			goto errexit;
		}
	}
errexit:
	status = status;
	//uart2_Printf("error status = %bu\r\n",status);
	return 0;
}

static void SendAck(unsigned char ack, unsigned char debug_id)
{
	debug_id = debug_id;
	if(ack == CHAR_ACK)
		;//uart2_Printf("send ACK %bu.\r\n",debug_id);
	else if(ack == CHAR_NAK)
		;//uart2_Printf("send NAK %bu.\r\n",debug_id);
	else if(ack == CHAR_CAN)
		;//uart2_Printf("send CAN %bu.\r\n",debug_id);
	sysprt->port()->send(&ack,1);
	thread_delay_ms(100);//(20);
//	FwDelayMs(1);

}




void xmodem_SendFile(unsigned int addr,unsigned int len)
{
	unsigned char state = 0,old_state = 0xff;
	unsigned char   buf[134];
	unsigned char i,sum,count = 0;
	unsigned int  oldticks,ticks;
	unsigned char nwrite,nread;
	unsigned char packno = 1;
	unsigned char retry_count = 0;
	unsigned char packlen = 132;
	unsigned char crc_mode = 0;
	unsigned short crc;
	while(1)
	{
		
		ticks = get_sys_ticks();
		if(old_state != state)
		{
			old_state = state;
			//uart2_Printf("--------------------%bd------------------\r\n",old_state);
		}
		switch(state)
		{
		case 0:
			retry_count =  0;
			oldticks = ticks;
			state = 1;
			break;
		case 1:
			if(ticks - oldticks < 10*1000ul)
			{
				nread = sysprt->port()->read_recv_buf(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_C)
					{
						crc_mode = 1;
						state = 2;
						packlen = 133;
						packno = 1;
					}
					else if(buf[0] ==  CHAR_NAK)
					{
						crc_mode = 0;
						state = 2;
						packlen = 132;
						packno = 1;
					}
					else if(buf[0] ==  CHAR_CAN)
					{
						printf("user cancels communication.\r\n");
						state = 100;
					}
					else
					{
						printf("error communication.\r\n");
						state = 100;
					}
				}
			}
			else 
			{
				oldticks = ticks;
				retry_count++;
				if(retry_count>= 5)
				{
					printf("can not receive response more than 50 second.exit\r\n");
					state = 100;
				}
			}
			break;
		case 2: /*send packets.*/
			if(len == 0)
			{
				SendAck(CHAR_EOT,0);
				retry_count = 0;
				oldticks = ticks;
				state = 4;
			}
			else
			{
				if(len >= 128) nwrite = 128;
				else
				{
					nwrite = len;
					memset(buf,0x1a,sizeof(buf));
				}
				buf[0] = CHAR_SOH;
				buf[1] = packno;
				buf[2] = packno ^ 0xffu;
				Storge_Read(&buf[3],nwrite,addr);
				if(crc_mode)
				{
					crc = crc_CalcCCITT16NibbleVer(&buf[3],128);
					buf[131] = crc >> 8;
					buf[132] = crc;
				}
				else 
				{
					for(i=0,sum=0;i<128;i++)
					{
						sum += buf[3+i];
					}
					buf[131] = sum;
				}
				sysprt->port()->send(buf,packlen);
				oldticks = ticks;
				retry_count = 10;
				state = 3;
			}
			break;
		case 3: /*check response*/
			if(ticks - oldticks < 10*1000ul)
			{
				nread = sysprt->port()->read_recv_buf(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_ACK)
					{
						addr += nwrite;
						len -= nwrite;
						packno++;
						state = 2;
					}
					else if(buf[0] ==  CHAR_NAK)
					{
						state = 2;
					}
					else if(buf[0] ==  CHAR_CAN)
					{
						printf("user cancels communication.\r\n");
						state = 100;
					}
					else
					{
						printf("error communication.\r\n");
						state = 100;
					}
				}
			}
			else 
			{
				oldticks = ticks;
				retry_count++;
				if(retry_count>= 5)
				{
					printf("can not receive response more than 50 second.exit\r\n");
					state = 100;
				}
			}
			break;
		case 4: /*Check the last ack.*/
			if(ticks - oldticks < 10*1000ul)
			{
				nread = sysprt->port()->read_recv_buf(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_ACK)
					{
						state = 100;
					}
					else if(buf[0] ==  CHAR_CAN)
					{
						printf("user cancels communication.\r\n");
						state = 100;
					}
					else
					{
						printf("error communication.\r\n");
						state = 100;
					}
				}
			}
			else 
			{
				oldticks = ticks;
				retry_count++;
				if(retry_count>= 5)
				{
					printf("can not receive response more than 50 second.exit\r\n");
					state = 100;
				}
			}
			break;
		case 100:
			goto errexit;
		default:
			break;
		}
	}
	errexit:
	;
}
/*---------------------------------------------------------------------*/

static void send_char( uint8_t dat )
{
		sysprt->port()->send( &dat,1 );
}

static uint8_t get_char( void )
{
	uint8_t dat = 0;
	while( 0 == sysprt->port()->read_recv_buf( &dat,1 ) );
	return dat;
}
/*-------------------------------------------------------------------------*/

#define XMODEM_SOH 0x01 // XModem ֡��ʼ��
#define XMODEM_ACK 0x06 // XModem ȷ��Ӧ���
#define XMODEM_NAK 0x15 // XModem ����Ӧ���
#define XMODEM_CAN 0x18 // XModem ȡ�������
#define XMODEM_EOT 0x04 // XModem ���������

// �����ݰ����͵�����
void xmodem_send_packet(unsigned char *packet, int len)
{
    int i;
    for (i = 0; i < len; ++i) {
      // ����һ���ֽ�
			sysprt->port()->send( &packet[i],1 );
    }
}

// ����һ�����ݰ�
int xmodem_recv_packet(unsigned char *packet)
{
    int cnt = 0, timeout = 0;
		int ret = 0;
    while (timeout < 200) 
		{
      ret = sysprt->port()->read_recv_buf( packet,1 );
			if( ret >0 )
			{
				++packet;
				++cnt;
				timeout = 0;
			}
			else 
			{
				// ����Ƿ�ʱ����ʱ���˳�����
				++timeout;
				if (timeout >= 200) 
				{
						return -1;
				}
			}
    }
    return cnt;
}

// ����CRCУ���
unsigned short calc_crc(unsigned char *buf, unsigned short len)
{
    unsigned short crc = 0;
    while (len-- > 0) {
        crc = crc ^ ((unsigned short)*buf++ << 8);
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

// ����һ�����ݰ�
int xmodem_send(unsigned char *data, int len, unsigned short seq)
{
    unsigned char packet[132], recv[3];
    unsigned short crc;
    int retry = 16, ret;
    // ֡��ʼ�� SOH
    packet[0] = XMODEM_SOH;
    // ��� SEQ �ͷ��� ~SEQ
    packet[1] = seq & 0xFF;
    packet[2] = (~seq) & 0xFF;
    // �������� DATA
    memcpy(packet + 3, data, len);
    // CRC У���
    crc = calc_crc(data, len);
    packet[len + 3] = (crc >> 8) & 0xFF;
    packet[len + 4] = crc & 0xFF;
    do {
        // �������ݰ�
        xmodem_send_packet(packet, len + 5);
        // �ȴ�����Ӧ��
        ret = xmodem_recv_packet(recv);
        if (ret == 1 && recv[0] == XMODEM_ACK) {
            // ���յ�ACK������ɹ������ش������ݵĳ���
            return len;
        }
        --retry;
    } while (retry > 0);

    // ������δ���ʧ�ܣ�����
    return -1;
}

// ����һ�����ݰ�
int xmodem_recv(unsigned char *data, int len, unsigned short seq)
{
    uint8_t packet[140], recv[3];
    uint16_t crc;
		uint8_t sdat = 0;
    int timeout, retry = 16, ret, datalen;
    do {
        // ����ACKӦ���
				sdat = XMODEM_ACK;
        xmodem_send_packet(&sdat, 1);
        // �ȴ��������ݰ�
        datalen = xmodem_recv_packet(packet);
        if (datalen == -1) {
            // ���ճ�ʱ������NAKӦ���
						sdat = XMODEM_NAK;
            xmodem_send_packet(&sdat, 1);
            --retry;
            continue;
        }
        if (packet[0] != XMODEM_SOH) {
            // ֡��ʼ���Ƿ�������NAKӦ���
						sdat = XMODEM_NAK;
            xmodem_send_packet(&sdat, 1);
            --retry;
            continue;
        }
        if (packet[1] != (seq & 0xFF) || packet[2] != (~seq & 0xFF)) {
            // ��Ż���ŷ���Ƿ�������NAKӦ���
						sdat = XMODEM_NAK;
            xmodem_send_packet(&sdat, 1);
            --retry;
            continue;
        }
        // ����CRCУ��Ͳ�����Ƿ���ȷ
        crc = calc_crc(packet + 3, 128);
        if ((crc >> 8) != packet[131] || (crc & 0xFF) != packet[132]) {
            // CRCУ��ͷǷ�������NAKӦ���
						sdat = XMODEM_NAK;
            xmodem_send_packet(&sdat, 1);
            --retry;
            continue;
        }

        // ��������DATA�Ƿ�������CANӦ������˳�
        if (datalen < 132) {
						sdat = XMODEM_NAK;
            xmodem_send_packet(&sdat, 1);
            return -1;
        }
        // �����ݿ�������������
        memcpy(data, packet + 3, len);
        // ����ACKӦ���
				sdat = XMODEM_ACK;
        xmodem_send_packet(&sdat, 1);
        return len;
    } while (retry > 0);

    // ������δ���ʧ�ܣ�����
    return -1;
}

// ��������
int xmodem_send_data(unsigned char *data, int len)
{
    unsigned char *buf = data, seq = 1;
    int ret, total = 0;
    while (len > 0) 
		{
        ret = xmodem_send(buf, (len > 128 ? 128 : len), seq);
        if (ret == -1) 
				{
            // ����ʧ�ܣ�����
            return -1;
        }
        buf += ret;
        len -= ret;
        total += ret;
        ++seq;
    }

    // ���ͽ�����־
		uint8_t sdat = XMODEM_EOT;
    xmodem_send_packet(&sdat, 1);
    return total;
}

// ��������
int xmodem_recv_data( int len)
{
    uint8_t buf[140];
		uint8_t seq = 1;
    int ret, total = 0;
    do 
		{
        ret = xmodem_recv(buf, (len > 128 ? 128 : len), seq);
        if (ret == -1) 
				{
            // ����ʧ�ܣ�����
            return -1;
        }
				//���ճɹ�����¼��ȥ
//        buf += ret;
        len -= ret;
        total += ret;
        ++seq;
    } while (len > 0);
		
    return total;
}

//void main()
//{
//    unsigned char sendbuf[] = "Hello, World!";
//    unsigned char recvbuf[1024];
//    int ret;
//    // ���ô��ڲ�����Ϊ9600bps
//    TMOD = 0x20;
//    TH1 = 0xFD;
//    TL1 = 0xFD;
//    TR1 = 1;
//    // ��������
//    ret = xmodem_send_data(sendbuf, strlen(sendbuf));
//    if (ret == -1) {
//        printf("Send data failed.\n");
//        while (1);
//    } else {
//        printf("Send data success, length: %d.\n", ret);
//    }
//    // ��������
//    memset(recvbuf, 0, sizeof(recvbuf));
//    ret = xmodem_recv_data(recvbuf, sizeof(recvbuf));
//    if (ret == -1) {
//        printf("Recv data failed.\n");
//        while (1);
//    } else {
//        printf("Recv data success, length: %d, content: \"%s\".\n", ret, recvbuf);
//    }

//    while (1);
//}
/*-------------------------------------------------------------------------*/

static void read_flash( uint8_t* buf,uint32_t len, uint32_t addr )
{
	volatile uint32_t *dat = (( volatile uint32_t *)( addr ));
	
	for( int i=0;i<len/4;i++ )
	{
		buf[i*4+0] = (uint8_t)(dat[i]			);
		buf[i*4+1] = (uint8_t)(dat[i]>>8	);
		buf[i*4+2] = (uint8_t)(dat[i]>>16	);
		buf[i*4+3] = (uint8_t)(dat[i]>>24	);
		
	}
	
}

static void erase_flash_4k( uint32_t addr )
{
	FLASH_Erase( addr );
}


static void write_flash( uint8_t *buf,uint32_t len,uint32_t addr )
{
//	uint8_t wbuf[128];
	
//	FLASH_Write( addr,(uint32_t*)(buf), len/4 );
}
/*-------------------------------------------------------------------------*/


#define check_argc_error( ival,jump ) \
	do{ \
		if( argc <ival ){ \
			sysprt->alog( "err: argc < %d \r\n",ival ); \
			goto jump; \
		} \
	}while(0);
/*-------------------------------------------------------------------------*/

//Ӧ�����

static int task_get_app1( char argc, char **argv)
{
	
	int ret = 0;
	
	sysprt->alog("task_get_app1\r\n");
	
	
	check_argc_error( 2,__usage_cmd );

	/*---------------------------------------*/
	if( strcmp(argv[1],"g") == 0 )
	{
		goto __do_get;
	}
	/*---------------------------------------*/
	else if( strcmp(argv[1],"d") == 0 )
	{
		goto __do_down;
	}
	/*---------------------------------------*/
	else if( strcmp(argv[1],"e") == 0 )
	{
		goto __do_erase;
	}
	/*---------------------------------------*/
	else
	{
		goto __usage_cmd;
	}
	
	return 0;
	
	/*---------------------------------------*/
__do_get:
	sysprt->alog("Yout want to read font data! \r\n");
	sysprt->alog("Please use xmodem ReceiveFile \r\n");
	thread_delay_ms( 100 );
	
	xmodem_SendFile(addr_app1,192*1024);		//192KB
	sysprt->alog("\r\nDevice SendFile success! \r\n");
	return 0;
	/*---------------------------------------*/
__do_down:
	sysprt->alog("Yout want to write font data! \r\n");
	sysprt->alog("Please use xmodem SendFile \r\n");
	thread_delay_ms( 100 );
	
	ret = xmodem_recv_data( 192*1024 );
	if (ret == -1) 
	{
			sysprt->alog("Recv data failed.\n");
			while (1);
	} 
	else 
	{
			sysprt->alog("Recv data success, length: %d \r\n", ret );
	}
	
//	xmodem_ReceiveFile( addr_app1 );
	sysprt->alog("\r\nDevice ReceiveFile success! \r\n");
	return 0;
	/*---------------------------------------*/
__do_erase:
	sysprt->alog("Yout want to EaserChip data! \r\n");
	sysprt->alog("Please wait... \r\n");
	
	for( int i=0;i<192/4;i++ )	//����ռ���192KB,ÿ�β���4KB
	{
		erase_flash_4k( addr_app1 + i*4096 );
	}
	
	sysprt->alog("EaserChip success! \r\n");
	return 0;
	/*---------------------------------------*/
__usage_cmd:
	sysprt->alog("->app1 <cmd>\r\n");
	sysprt->alog("app1 g/d/e,(get/down/erase)\r\n" );
	return -1;

}
cmd_util_t __cmd_get_app1={
	.name 	= "app1",
	.usage 	= "app1 g/d/e",
	.text 	= "app1 g/d/e,(get/down/erase)",
	.app 		= task_get_app1
};
