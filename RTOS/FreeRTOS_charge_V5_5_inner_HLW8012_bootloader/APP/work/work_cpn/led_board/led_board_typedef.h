

/*----------------------------------------------------------------------------------*/

//���滺����������Ʒ��ֵ�Ļ�����
/*
bit[9 : 0]	�ư�1
bit[23:14]	�ư�2
*/
typedef struct{
	uint8_t led[3];					//�ư壬��ʾLED
}ledb_buf_t,*ledb_buf_pt;



enum{
	_e_mode_standing=0,			//����״̬
	_e_mode_charging,				//�����
	_e_mode_charge_full,		//����״̬
	
};//

/*----------------------------------------------------------------------------------*/
//�����������ĺ���

typedef struct{
	void (*init)( void );
	
	//��������״̬
	void (*set_net_led)( uint8_t net );
	
	//���ñ���˿״̬
	void (*set_fuse_led)( uint8_t fuse );
	
	
	//�����л���ʾģʽ��road,mode,dat0,dat1
	void (*mode_req)( uint8_t road,uint8_t mode,uint32_t dat0,uint32_t dat1 );
	
	void (*work)( void );
	
	
}led_board_t,*led_board_pt;



