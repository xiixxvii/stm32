

#include "./driver_port/fk_drv_port_typedef.h"


/*--------------------------------------------------------------------------------*/
//���λ�����
#define Loop_MAXBUFLEN	(1024)
#if (Loop_MAXBUFLEN>4096)
#error "Rx buffer must be less than 512 bytes because of head/rear data size is 8-bit unsigned char."
#endif
typedef struct{

	uint32_t head;
	uint32_t rear;
	uint8_t buffer[Loop_MAXBUFLEN];
}loopbuf_t,*loopbuf_pt;

typedef struct{
	int (*read)(loopbuf_pt ploop,void *vbuf,unsigned char len);
	void (*write)(loopbuf_pt ploop,uint8_t dat);
	void (*reset)( loopbuf_pt ploop );
	loopbuf_pt (*create)(void);
}loop_t,*loop_pt;


/*--------------------------------------------------------------------------------*/
//gpio 
typedef struct{
	int (*read)( int pin);
	void (*write)( int pin, int value);
	void (*set_mode)( int pin, int mode);
	int (*attach_irq)( int pin,int mode,pin_callback_t cb,void *args);
	int (*detach_irq)( int pin );
	int (*irq_enable)( int pin,int enabled);
}drv_gpio_t,*drv_gpio_pt;

/*--------------------------------------------------------------------------------*/


typedef struct{
	void (*init)( uint32_t baudrate );
	void (*start)( void );
	void (*stop)( void );
	//��������
	void (*send)(uint8_t *buf,uint8_t len);						//ֱ�ӷ���
	void (*req_data_send)(uint8_t *buf,uint8_t len);	//������
	void (*deal_data_send)(void);											//�������ݷ���
	//��������
	uint32_t (*read_recv_buf)(uint8_t *buf,uint8_t len);
	void (*clr_rbuf)( void );
	
	//�����ж�
	void (*en_irq)( void );														/*���ж�*/
	void (*dis_irq)( void );													/*���ж�*/

	
}uart_t,*uart_pt;

/*--------------------------------------------------------------------------------*/
//input_bus
//drv_input_bus.c
typedef struct{
	void (*init)(void);
	void (*refresh)(void);
	uint8_t (*get_bit)(uint8_t pin);
	uint8_t (*get_byte)(uint8_t num);
}input_bus_t,*input_bus_pt;

/*--------------------------------------------------------------------------------*/

typedef struct{
	void (*set_bit)( uint8_t ibit );
	void (*clr_bit)( uint8_t ibit );
	int  (*get_bit)( uint8_t ibit );
}bit_ctrl_t,*bit_ctrl_pt;
/*--------------------------------------------------------------------------------*/

//io_bus
typedef struct{
	void (*init)( uint8_t chip );			//��ʼ��io bus
	bit_ctrl_pt in;
	bit_ctrl_pt out;
	void (*work)( void );							//ѭ����ҵ
}io_bus_t,*io_bus_pt;
/*--------------------------------------------------------------------------------*/
//hc595_bus
typedef struct{
	uint8_t dat;
	uint8_t clk;
	uint8_t ud;
	uint8_t cs;
	uint32_t index;		//��ʱֵ
}hc595_pin_t,*hc595_pin_pt;


typedef struct{
	void (*init) ( hc595_pin_pt bus );				//��ʼ�� bus ����
	void (*write)( hc595_pin_pt bus, uint8_t* buf,uint8_t len );
	void (*update)( hc595_pin_pt bus );							
}hc595_opt_t,*hc595_opt_pt;



typedef struct{
	void (*init)( uint8_t pin,uint8_t type );
	void (*out)( uint8_t pin, uint8_t sta );
	int (*in)( uint8_t pin );
	void* (*opt)( uint8_t pin,uint8_t action,void* dat );
}iopin_t,*iopin_pt;

typedef struct{
	void (*init)(void);
	int (*read)( uint8_t SLV_ADDR,uint8_t addr,uint8_t *buf,uint8_t len );
	int (*write)( uint8_t SLV_ADDR,uint8_t addr,uint8_t *buf,uint8_t len );
	
}eeprom_t,*eeprom_pt;

/*--------------------------------------------------------------------------------*/
//pwm_bus
typedef struct{
	void (*init)( void );																				//��ʼ��ģ��
	void (*set_ctick)( uint32_t road, uint16_t ctick );					//�������ڿ��
	void (*set_htick)( uint32_t road, uint16_t htick );					//���øߵ�ƽ����
	
	void (*start)( uint32_t road );															//����PWM���
	void (*stop)( uint32_t road );															//�ر�PWM���
	void (*set_isr)( uint32_t road,pwm_isr func );							//�����жϵ���
	
}pwm_bus_t,*pwm_bus_pt;

/*--------------------------------------------------------------------------------*/
//adc_bus
typedef struct{
	void (*init)(void);
	void (*enable)(uint8_t road);
	void (*disable)(uint8_t road);
	uint32_t (*read)(uint8_t road);
	void (*work)(void);

}adc_bus_t,*adc_bus_pt;

/*--------------------------------------------------------------------------------*/
typedef struct{
	void (*init)(void);
	uint32_t (*read_pul)( int road );
	void (*work)( void );
}hlw_t,*hlw_pt;
/*--------------------------------------------------------------------------------*/
//define rtc_dt_t RTC_DateTime
typedef RTC_DateTime rtc_dt_t;
typedef struct{
	void (*init)( rtc_dt_t* dt,int mINT,int sINT );
	void (*start)( void );
	void (*stop)( void );
	void (*read)( rtc_dt_t* dt );
	
}rtc_t,*rtc_pt;
/*--------------------------------------------------------------------------------*/


typedef struct{
	void (*init)(void);
	void (*set_pin)(int road,int pin);
	void (*config)( int road,int filter,uint32_t freq );
	
	void (*start)(int road);
	void (*stop)(int road);
	
	void (*clr_val)(int road);

	uint32_t (*cat_val)(int road);
	int (*cat_pin)(int road);
	
	void (*work)( void );
	
}drv_coder_t,*drv_coder_pt;

/*--------------------------------------------------------------------------------*/


