#include "framework.h"
/*------------------------------------------------------------------------*/


static uint8_t sysfb[LCD_YPixel][LCD_XPixel/8];

static void init( void )
{
	
	for( int y=0;y<LCD_YPixel;y++ )
		for( int x=0;x<LCD_XPixel;x++ )
		{
			sysfb[y][x] = 0;
		}
		

}


static void pixel(uint8_t x,uint8_t y,uint8_t color)							
{
	uint8_t *LCDFrameBuff = (uint8_t*)sysfb;
	//��������ֵ��ȷ������λ�ڵڼ����ֽں�����ֽڵĵڼ�λ
	uint8_t x_word,x_bit;
	x_word = x>>3;	//ʹ��������ȡ������	x_word = x/8;
	x_bit  = x&7; 	//ʹ�ð�λ����ȡ������
	
	if((x<LCD_XPixel)&&(y<LCD_YPixel))
	{
		//�޸Ĵ��ֽڵ�����
		if( color )
		{
			LCDFrameBuff[y*16+x_word] |=(1<<(7-x_bit)) ;
		}
		else
		{
			LCDFrameBuff[y*16+x_word] &=~(1<<(7-x_bit)) ;
		}
	}
	//y*16��ʾ�ڼ��е����׵�ַ,1��16���ֽ�
					//j��ʾ,�ַ���ˮƽ������ʾ���ڼ����ֽڣ�һ�������ֽ�
					//x��ʾ,�ַ���ˮƽ�����ϴӵڼ����ֽڿ�ʼ��ʾ
					//i��ʾ,�ַ��ڴ�ֱ������ʾ���ڼ���
					//LCDFrameBuff[0]Ϊ�ַ���			 	��0�е��׵�ַ
					//LCDFrameBuff[0+y*16]					Ϊ�ַ���	��y�п�ʼ��ʾ���׵�ַ
					//LCDFrameBuff[0+y*16+j]				Ϊ�ַ�	�ڵ�y����ʾ�ĵ�x_word���ַ��׵�ַ
}

static void show_ascii(uint8_t x,uint8_t y, uint8_t ch)
{
	uint8_t i;
	uint8_t *pch=0;
	pch = ( uint8_t* )(&ascii_1608[ch*16]);//��ȡASCII����׵�ַ
	uint8_t x_word = x>>3;	//ʹ��������ȡ������	x_word = x/8;
	for( i=0;i<16;i++ )
	{
		sysfb[ y+i ][x_word]= pch[i];
	}
		
	
}

static int show_ascii_str(uint8_t x,uint8_t y,char ch[])
{
	uint8_t i,len;
	len = strlen(ch);
	/*------------������--------------------------------------------*/
	if(len>16)	return 0;	//�������������ܴ�����	
	if((16-x)<len)	return 0;//�ַ���̫����������ʼ����֮��Ŀռ���ʾ����
	if(y>=48)		return 0;
	
	/*------------������ʾ--------------------------------------------*/
	for(i=0;i<len;i++)
	{
		show_ascii(x,y,ch[i]);
		x += 8;
	}
	return 1;	
}

static void show( void )
{
	int yline = LCD_YPixel;
	int xword	= LCD_XPixel/8;
	uint8_t dat = 0;

	printf("\r\n");
	for( int l=0;l<yline;l++ )
	{	
		for( int x=0;x< xword;x++ )
		{
			dat = sysfb[ l ][ x ];
			for( int i=0;i<8;i++ )
			{
				if( dat&0x80 ) 	printf("X");
				else						printf(" ");
				
				dat = dat<<1;
			}
		}
		printf("\r\n");
	}
}
static dot_ui_t m_dot_ui={
	init,
	pixel,
	show_ascii,
	show_ascii_str,
	show
};
dot_ui_pt dot_ui = &m_dot_ui;
