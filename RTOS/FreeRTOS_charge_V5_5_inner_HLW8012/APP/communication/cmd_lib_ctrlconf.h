


typedef struct{
	uint8_t sCode;					//֡ͷ
	uint8_t cCode;					//������
	uint8_t module;					//ģ��
	uint8_t action;					//��Ϊ
	uint8_t arg[6];					//���ݿ�
	uint8_t sum;						//У���
	uint8_t eCode;					//֡β
}cmd_ctrl_conf_t,*cmd_ctrl_conf_pt;
enum{
	_ctrl_conf_outputbus = 0,
	_ctrl_conf_intputbus,
};

