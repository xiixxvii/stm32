
#define task_handle_t TaskHandle_t

typedef struct{

	task_handle_t* handle;
	char* name;				//��������
	char* cn_name;		//��������
	int stack_size;		//��ջ�ߴ�
	int priority;			//�������ȼ�
	void (*create)(void* argc,void** argv);			//����
	void (*del)(void);													//ɾ��
	void (*suspend)(void);											//��ͣ
	void (*recovery)(void);											//�ָ�
}task_body_t,*task_body_pt;

#define __task_body_quote_to( name ) \
	task_body_pt name = &m_task_body;

#define __task_body_quote( name ) \
	extern task_body_pt name;


#define __task_body_start \
	static task_handle_t handle;							 		\
	static void task_entry(void *arg);						\
	static void create(void* argc,void** argv); 

#define __task_body_end \
	static task_body_pt task_body = &m_task_body; \
	static void create(void* argc,void** argv) \
	{ \
		xTaskCreate( \
			task_entry, 												/*��ں���*/ \
			(const char *)task_body->name, 			/*��������*/ \
			task_body->stack_size,							/*��ջ��С*/ \
			NULL,																/*�������*/ \
			task_body->priority,								/*���ȼ�*/   \
			task_body->handle										/*������*/ \
		); \
	}
	


