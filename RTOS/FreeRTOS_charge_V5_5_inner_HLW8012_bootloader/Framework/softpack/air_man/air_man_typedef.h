


typedef struct{
	char* name;						//AT�豸������
	uart_pt uart;					//����ʲô����
	uint8_t *rxbuf;				//���ջ�����
	uint32_t rxlen;				//�������ݳ���
	
	//ƥ�䲨����
	int (*auto_baudrate)( void* airdev,int argc,char** argv );
	//AT����
	int (*cmd_AT)				( void* airdev,int argc,char** argv ) ;
	int (*cmd_AT_CGMI)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CGMR)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CPIN)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CSQ)		( void* airdev,int argc,char** argv );
	int (*cmd_AT_CREG)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CGATT)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CSTT)	( void* airdev,int argc,char** argv );
	
	int (*cmd_AT_CIICR)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIFSR)	( void* airdev,int argc,char** argv );
	
	int (*cmd_AT_CIPMODE)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIPMUX)	( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIPSTART)( void* airdev,int argc,char** argv );
	int (*cmd_AT_CIPCLOSE)( void* airdev,int argc,char** argv );
	
	int (*cmd_AT_RESET)( void* airdev,int argc,char** argv );
	
	
//	int (*cmd_AT_CIPQSEND)( void* airdev );

	
	
	
}air_dev_t,*air_dev_pt;

typedef struct{
	air_dev_pt (*create_dev)( uart_pt uart );
	
	void (*delete_dev)( air_dev_pt dev	);

	
	void (*send_str)(air_dev_pt dev,char *string);
	void (*clr_rxbuf)( air_dev_pt dev );
	void (*ptf_rxbuf)( air_dev_pt dev , int hex );
	
	
}air_man_t,*air_man_pt;

