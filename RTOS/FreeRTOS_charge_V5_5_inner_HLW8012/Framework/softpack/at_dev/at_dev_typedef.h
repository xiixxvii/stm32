



/*
at dev �����
(1)���Դ���������AT�豸
(2)�������ð�һ��UART
(3)һ���豸����ע�����ATָ��
(4)������������AT����+���ݵĸ�ʽ�����ô��ڵײ㷢������
(5)���ͺ����������ģ�����
��������֮�󣬻��AT�豸����
����ȵ��س����ź��ж�AT�����ǳɹ�����ʧ��
���5sû�еȵ�AT���أ����׳����ʴ���


*/

typedef int (*cmd_cb_t)( uint8_t buf,uint32_t len );


#define cmd_is_unreg	0
#define cmd_is_reg		1


typedef struct{
	char* name;						//ATָ������	
	uint32_t number;			//���
	cmd_cb_t callback;		//���ջص�
	char* usage;					//�÷�
}at_cmd_t,*at_cmd_pt;

typedef struct{
	uint32_t reg;					//�Ƿ�ע��
	at_cmd_pt body;				//ָ������
}ele_t,*ele_pt;


typedef struct{
	char* name;						//AT�豸������
	uint32_t max_cmd;			//���֧�ֶ��ٸ�AT����
	uart_pt uart;					//����ʲô����
	uint8_t *rxbuf;				//���ջ�����
	uint32_t rxlen;				//�������ݳ���
	
}at_dev_t,*at_dev_pt;

typedef struct{
	at_dev_pt (*create_dev)( uart_pt uart,uint32_t max_cmd );
	void (*delete_dev)( at_dev_pt dev	);

	//ƥ�䲨����
	int (*aoto_baudrate)( at_dev_pt dev,uint32_t cto );			
	
	//��������
	int (*send_cmd)( at_dev_pt dev,char* at,uint32_t dto,uint32_t cto );
	
	//��ӡ���ջ�����
	void (*printf_rxbuf)( at_dev_pt dev , int hex );
	

}at_man_t,*at_man_pt;

