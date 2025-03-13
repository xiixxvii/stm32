
#include "framework.h"

#define PRT_LOG(X...)	sysprt->alog//dprintf(X)	

//�������������������
static int dump( char* buf,char* str )
{
	int index = 0;
	while( str[index] != '\0' )
	{
		buf[index] = str[index];
		index++;
	}
	return index;
}

//��һ������������һ�����ݳ���
//srcbuf:		����Դ
//destbuf:	Ŀ���ַ
//start:		��ʼλ��
//end:			������ַ
static void copy_section( uint8_t* srcbuf,uint8_t* destbuf,int start,int end )
{
	int i = 0;
	for( i=start;i<end;i++ )
	{
		destbuf[ i ] = srcbuf[i];
	}
	
}


static char *my_strstr(const char *haystack, const char *needle) {
    // ��� needle Ϊ�գ����� haystack
    if (*needle == '\0') {
        return (char *)haystack;
    }

    // ���� haystack �ַ���
    for (; *haystack != '\0'; haystack++) {
        const char *h = haystack;
        const char *n = needle;

        // �Ƚ� haystack �еĵ�ǰ�ַ��� needle
        while (*h == *n && *n != '\0') {
            h++;
            n++;
        }

        // ������� needle �ַ���ƥ��ɹ�
        if (*n == '\0') {
            return (char *)haystack;
        }
    }

    // û���ҵ� needle
    return NULL;
}


//�ַ��������Ӵ�
//�ɹ��������ַ������Ӵ���ƫ����
//ʧ�ܣ� ����-1
static int findstr( char* haystack,char* needle  )
{
	char *a = NULL;
//	a = strstr( haystack,needle );
	a = my_strstr( haystack,needle );
	if( a < 0  )	return -1;
	return ((uint32_t)a - (uint32_t)haystack);
}



//�鿴�ж��ٸ��Ӵ���������Ӧ�Ӵ�ƫ���������offset_buf��
//haystack 	����
//needle 		�Ӵ�
//offset_buf			���ƫ���������飬ƫ�������65535
//len							��������ܴ�Ŷ��ٸ�����

//���� �Ӵ��ĸ���
static int get_substr_index( char* haystack,char* needle,int offset_buf[],int len )
{
	char *a=NULL;
	int n=0,l=0;
	l = strlen( needle );
	a = strstr( haystack,needle );
	if( a < 0  )	return 0;
	offset_buf[n] = 	((uint32_t)a - (uint32_t)haystack);
	
	while( a!=NULL )
	{
		n++;
		if( len <= n ) return n;
		a = strstr( a+l,needle );
		if( a < 0  )	return n;
		offset_buf[n] = 	((uint32_t)a - (uint32_t)haystack);
	}
	return n;
}



//��ȡ�ַ���
static int intercept( char* src,char* dest,char* sstr,char* estr )
{
	//���ݱ����ֶ����ƺ���һ���ֶ����Ƽ�ס��pos_field���2���ֶε���ʼλ��
	int pos_field[2];
			
	char *pdat = dest;
	int index = 0;

	int start = 0,end = 0;

	pos_field[0] = findstr( (char*)src, sstr );
	if( pos_field[0] < 0 ) return -1; 
	
	pos_field[1] = findstr( (char*)src, estr );
	if( pos_field[1] < 0 ) return -1; 
	
	//��ȡ�ַ�������
	start =  pos_field[0] + strlen( sstr );
	end = pos_field[1] - 1;
	for( index = start; index <= end; index++  )
	{
		*pdat = src[index]; 
		pdat++;
	}
	*pdat = '\0';
	
	return 0;
}

static char* strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char *end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}

/*--��ȡһ�����ȵ��ַ������бȽ�-----------------------------------*/
static uint8_t strcmpn(char *str1, char *str2,unsigned char len)
{
	unsigned char i=0;
	for(i=0;i<len;i++)
	{
		if(str1[i] != str2[i])
			return 0;
	}
	
	return 1;
}


//�и��ַ���
static int split_string_n(int max,char *str, char **result, const char *delimiters) 
{
    int count = 0;
    char *token;
    char *saveptr; // ���ڱ��������ĵ�ָ��

    // ʹ�� strtok_r ��������ַ���
    token = strtok_r(str, delimiters, &saveptr);

    while (token != NULL) {
        result[count] = token; // ���浱ǰ�� token
        count++;
				if( count>=max ) break;
        // ��ȡ��һ�� token
        token = strtok_r(NULL, delimiters, &saveptr);
    }

    return count;
}



extern void url_encode(char src[],char dest[]);


static strlib_t do_strlib = { 
	dump,
	copy_section,
	findstr,
	get_substr_index,
	intercept,
	strsep,
	strcmpn,
	url_encode,
	split_string_n,
};


strlib_pt strlib = &do_strlib;






