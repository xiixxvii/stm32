#include "framework.h"

/*

���ļ����˼�룺
->���ṩHC595�������������
->ʵ�ֲ���HC595Ӳ��
��Ϊ��ͬ����Ŀ��HC595���߶�Ӧ������ܻ�仯
���ԣ�
(1)��DRV�ļ���ֻ�ǲ���HC595�ķ���
(2)������DRVP�ļ���ֻ������HC595��Ӧʲô��·
��APP������������һ��HC595���ߣ�������ܵ�λ��״̬û���κι�ϵ
��λ/����ܵƸ�����APP������

���ļ������Ű�ṹ
(1)ʵ��hc595�Ĳ�������
(2)ʵ��hc595����ӳ�䵽�ײ�Ӳ������
*/


#define wide_rck 50
#define wide_sck 20

enum{
	hc_road_0 = 0,
	hc_road_1,
	hc_road_2,
	hc_road_3,
	MaxHcRoad,
};
static volatile uint32_t d[MaxHcRoad]={0,0,0};


static hc595_pin_t m_LED_BUS = { IO_HC_DAT,IO_HC_CLK,IO_HC_UD,0,hc_road_0 };

hc595_pin_pt LED_BUS = &m_LED_BUS;



//��ʼ�� bus ����
static void init( hc595_pin_pt bus )
{
//	drvp_hc595_bus[1] = &do_drvp_hc595_bus_1;
//	drvp_hc595_bus[2] = &do_drvp_hc595_bus_2;

	gpio->set_mode( bus->dat,PIN_MODE_OUTPUT );
	gpio->set_mode( bus->clk,PIN_MODE_OUTPUT );
	gpio->set_mode( bus->ud,PIN_MODE_OUTPUT );
	gpio->set_mode( bus->cs,PIN_MODE_OUTPUT );
	
	
}


static void write_byte( hc595_pin_pt bus,uint8_t dat )
{
	int road = bus->index;
	unsigned char i;
	for(i=0;i<8;i++)
	{
		//�����������
		if (dat&0x80) 
			gpio->write( bus->dat,PIN_HIGH );
		else 
			gpio->write( bus->dat,PIN_LOW );
		dat<<=1;
		//���������أ��Ĵ�����λ	
		
		gpio->write( bus->clk,PIN_LOW );				//drvp_hc595_bus[road]->CLK_L();
		d[road] = wide_sck;while( d[road] --);
		gpio->write( bus->clk,PIN_HIGH );				//drvp_hc595_bus[road]->CLK_H();

		d[road] = wide_sck;while( d[road] --);
	}
}

static void write(  hc595_pin_pt bus, uint8_t* buf,uint8_t len )
{
	for( int i=0;i<len;i++ )
		write_byte( bus, buf[ len-1-i ] ); //�ȴ�Զ����ʼ������
}

static void update( hc595_pin_pt bus )
{
	int road = bus->index;
	gpio->write( bus->ud,PIN_LOW );
	d[road] = wide_rck;while( d[road]--);
	gpio->write( bus->ud,PIN_HIGH );
	d[road] = wide_rck;while( d[road] --);
	
}

static hc595_opt_t do_hc595_bus={init,write,update};

hc595_opt_pt hc595_opt = &do_hc595_bus;


