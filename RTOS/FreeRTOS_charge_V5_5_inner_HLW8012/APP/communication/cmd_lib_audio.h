
typedef struct{
	uint8_t sCode;					//֡ͷ
	uint8_t cCode;					//������
	uint8_t dev;						//�豸
	uint8_t action;					//��Ϊ
	uint8_t arg[6];					//���ݿ�
	uint8_t sum;						//У���
	uint8_t eCode;					//֡β
}cmd_audio_t,*cmd_audio_pt;
enum{
	_audio_init_dev = 0,
	_audio_set_volume,
	_audio_set_gingle_cycle,
	_audio_set_play_now,
	_audio_set_play_music,
};




