


//�ֽڿ�
typedef struct{
	//�����ӿڣ���������ߴ�����
	void (*set_bit)( uint8_t* buf,uint8_t ibit );
	void (*clr_bit)( uint8_t* buf,uint8_t ibit );
	uint8_t (*read_byte)( uint8_t* buf,uint8_t num );
	//���½ӿڴ�����ߴ�����
	void (*lset_bit)( int max_len,uint8_t* buf,uint8_t ibit );
	void (*lclr_bit)( int max_len,uint8_t* buf,uint8_t ibit );
	uint8_t (*lread_byte)( int max_len,uint8_t* buf,uint8_t num );
	
}byte_array_t,*byte_array_pt;







