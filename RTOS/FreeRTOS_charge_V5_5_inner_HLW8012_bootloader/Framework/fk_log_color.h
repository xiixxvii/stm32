
/*�������ǰ��ɫ*/
#define PRINT_FONT_BLA 	"\033[30m" //��ɫ
#define PRINT_FONT_RED  "\033[31m" //��ɫ
#define PRINT_FONT_GRE  "\033[32m" //��ɫ
#define PRINT_FONT_YEL  "\033[33m" //��ɫ
#define PRINT_FONT_BLU  "\033[34m" //��ɫ
#define PRINT_FONT_PUR  "\033[35m" //��ɫ
#define PRINT_FONT_CYA  "\033[36m" //��ɫ
#define PRINT_FONT_WHI  "\033[37m" //��ɫ
/*�����������ɫ*/ 
#define PRINT_BACK_BLA  "\033[40m" //��ɫ
#define PRINT_BACK_RED  "\033[41m" //��ɫ
#define PRINT_BACK_GRE  "\033[42m" //��ɫ
#define PRINT_BACK_YEL  "\033[43m" //��ɫ
#define PRINT_BACK_BLU  "\033[44m" //��ɫ
#define PRINT_BACK_PUR  "\033[45m" //��ɫ
#define PRINT_BACK_CYA  "\033[46m" //��ɫ
#define PRINT_BACK_WHI  "\033[47m" //��ɫ
/*�����������*/
#define PRINT_ATTR_REC  "\033[0m" //�����������Ե�ȱʡ���� 
#define PRINT_ATTR_BOL  "\033[1m" //���ô��� 
#define PRINT_ATTR_LIG  "\033[2m" //����һ������(ģ���ɫ��ʾ������ɫ) 
#define PRINT_ATTR_LIN  "\033[4m" //�����»���(ģ���ɫ��ʾ������ɫ) 
#define PRINT_ATTR_GLI  "\033[5m" //������˸ 
#define PRINT_ATTR_REV  "\033[7m" //���÷���ͼ�� 
#define PRINT_ATTR_THI  "\033[22m" //����һ���ܶ� 
#define PRINT_ATTR_ULIN  "\033[24m"//�ر��»��� 
#define PRINT_ATTR_UGLI  "\033[25m"//�ر���˸ 
#define PRINT_ATTR_UREV  "\033[27m"//�رշ���ͼ��


#define SYS_LogOUT(X...) printf(X)
#define APP_LogOUT(X...) printf(X)
#define DRV_LogOUT(X...) printf(X)
#define CPM_LogOUT(X...) printf(X)

/*----------------------------------------------------------------------------------*/
//#define aprintf(X...){APP_LogOUT(PRINT_FONT_YEL);APP_LogOUT(X);APP_LogOUT(PRINT_ATTR_REC);}
//#define eaprintf(X...){APP_LogOUT(PRINT_FONT_RED);APP_LogOUT(X);	APP_LogOUT(PRINT_ATTR_REC);}

//#define alogprintf(color,X...){APP_LogOUT(color);APP_LogOUT(X);APP_LogOUT(PRINT_ATTR_REC);}
///*----------------------------------------------------------------------------------*/
//#define dprintf(X...){ DRV_LogOUT(PRINT_FONT_GRE);DRV_LogOUT(X);DRV_LogOUT(PRINT_ATTR_REC);}
//#define edprintf(X...){DRV_LogOUT(PRINT_FONT_RED);DRV_LogOUT(X);DRV_LogOUT(PRINT_ATTR_REC);}


//#define dlogprintf(color,X...){DRV_LogOUT(color);DRV_LogOUT(X);DRV_LogOUT(PRINT_ATTR_REC);}



