



typedef struct{
	//�������
	int (*dump)( char* buf,char* str );
	
	//�ӻ�������ĳ�����临������
	void (*copy_section)( uint8_t* srcbuf,uint8_t* destbuf,int start,int end );
	
	//�ַ��������Ӵ�
	int (*findstr)( char* haystack,char* needle  );
	
	//���ַ��������ȡ�Ӵ���ƫ�����������سɹ���ȡ���Ӵ���
	int (*get_substr_index)( char* haystack,char* needle,int offset_buf[],int len );
	
	//��ȡ�ַ���
	int (*intercept)( char* src,char* dest,char* sstr,char* estr );
	
	char* (*strsep)(char **s, const char *ct);
	
	uint8_t (*strcmpn)(char *str1, char *str2,unsigned char len);
	
	//url
	void (*url_encode)(char src[],char dest[]);

	
}strlib_t,*strlib_pt;


