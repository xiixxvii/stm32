
//------------------------------------------------------------------------------
//ƥ�䲨����
extern int air724_auto_baudrate( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//������ֹ֮ǰ�Ĳ��Ϸ�����
extern int air724_cmd_AT( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//��ȡģ�鳧����Ϣ
extern int air724_cmd_AT_CGMI( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//��ȡ�̼��汾��Ϣ
extern int air724_cmd_AT_CGMR( void* airdev,int argc,char** argv );

//��ѯ���Ƿ���
extern int air724_cmd_AT_CPIN( void* airdev,int argc,char** argv );
//��ѯ�����ź�����
extern int air724_cmd_AT_CSQ( void* airdev,int argc,char** argv );

//��ѯ����ע��״̬
extern int air724_cmd_AT_CREG( void* airdev,int argc,char** argv );

//��ѯ����GPRS����
extern int air724_cmd_AT_CGATT( void* airdev,int argc,char** argv );

//�����Զ���ȡAPN
extern int air724_cmd_AT_CSTT( void* airdev,int argc,char** argv );

//�����ƶ�������������ܻ�ȡ��IP
extern int air724_cmd_AT_CIICR( void* airdev,int argc,char** argv );

//��ѯIP��ֻ�л�ȡ��IP���������
extern int air724_cmd_AT_CIFSR( void* airdev,int argc,char** argv );

//------------------------------------------------------------------------------
//����͸��ģʽ
extern int air724_cmd_AT_CIPMODE( void* airdev,int argc,char** argv );
//����IPΪ������
extern int air724_cmd_AT_CIPMUX( void* airdev,int argc,char** argv );

//����TCP/UDP����
extern int air724_cmd_AT_CIPSTART( void* airdev,int argc,char** argv );
//�ر�TCP/UDP����
extern int air724_cmd_AT_CIPCLOSE( void* airdev,int argc,char** argv );

extern int air724_cmd_AT_RESET( void* airdev,int argc,char** argv );

