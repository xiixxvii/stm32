/*
 * Copyright (c) 2024, Synwit, <linzh@synwit.cn>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fal.h>
#include <string.h>
#include "SWM341.h"

/* note: align ������ 2 ��ָ���� */

#define IS_ALIGN(val, align)        ( ( !( (val) & ( (align) - 1 ) ) ) ) //&& (val) )

#define ALIGN_UP(val, align)        ( ( (val) + (align) - 1 ) & ~( (align) - 1 ) )

#define ALIGN_DOWN(val, align)      ( (val) & ~( (align) - 1 ) )


static int init(void)
{
    /* SFC ʹ��ר�õ� FSPI(Flash SPI)�ӿ����� SPI FLASH */
    PORT_Init(PORTD, PIN5, PORTD_PIN5_FSPI_SCLK, 0);
    PORT_Init(PORTD, PIN6, PORTD_PIN6_FSPI_SSEL, 0);
    PORT_Init(PORTD, PIN8, PORTD_PIN8_FSPI_MOSI, 1);
    PORT_Init(PORTD, PIN7, PORTD_PIN7_FSPI_MISO, 1);
    PORT_Init(PORTD, PIN3, PORTD_PIN3_FSPI_DATA2, 1);
    PORT_Init(PORTD, PIN4, PORTD_PIN4_FSPI_DATA3, 1);
    
    SFC_InitStructure SFC_initStruct;
    uint8_t width = 4;
    switch (width)
    {
    default:
        //break;//continues
    case 1:
        SFC_initStruct.ClkDiv = SFC_CLKDIV_2;
        SFC_initStruct.Cmd_Read = 0x03;
        SFC_initStruct.Width_Read = SFC_RDWIDTH_1;
        SFC_initStruct.Cmd_PageProgram = 0x02;
        SFC_initStruct.Width_PageProgram = SFC_PPWIDTH_1;
        break;

    case 2:
        SFC_initStruct.ClkDiv = SFC_CLKDIV_2;
        SFC_initStruct.Cmd_Read = 0xBB;
        SFC_initStruct.Width_Read = SFC_RDWIDTH_2;
        SFC_initStruct.Cmd_PageProgram = 0x02;
        SFC_initStruct.Width_PageProgram = SFC_PPWIDTH_1;
        break;

    case 4: /* ���� SPI Flash �� Quad SPI Mode(��鿴��Ӧ�� SPI Flash �ͺ��Ƿ�֧�� Quad SPI, �ҿ���ָ��/ʱ���Ƿ�����������ʹ�õ�һ��) */
        SFC_initStruct.ClkDiv = SFC_CLKDIV_2; /* ��Ƶ 120MHz ���� 1 ��Ƶ, ��Ƶ 120MHz ���� 2 ��Ƶ */
        SFC_initStruct.Cmd_Read = 0xEB;
        SFC_initStruct.Width_Read = SFC_RDWIDTH_4;
        SFC_initStruct.Cmd_PageProgram = 0x32;
        SFC_initStruct.Width_PageProgram = SFC_PPWIDTH_4;
        break;
    }
    SFC_Init(&SFC_initStruct);
    
    /* SFC ���ֻ�ܶ�ȡ uint32_t ��Χ�ڵ� id */
    uint32_t id = SFC_ReadJEDEC();
    if (id == 0 || id == 0xFFFFFFFF)
    {
         while (1) __NOP(); //����Ӳ���Ƿ���Ч
    }
        
    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    long addr = nor_flash0.addr + offset;
    
    size_t word_num = size >> 2;
    size_t word_align_size = size - ALIGN_DOWN(size, 4);
    size_t addr_align = ALIGN_UP(addr, 4) - addr;
    size_t buff_align = ALIGN_UP((uint32_t)buf, 4) - (uint32_t)buf;
    
    uint32_t temp[1] = {0};
    
    if (word_num > 0)
    {
        /* ��ַ�ֶ��� */
        if (addr_align > 0)
        {
            SFC_Read(ALIGN_DOWN(addr, 4), temp, 1);
            memcpy(buf, &((uint8_t *)temp)[4 - addr_align], addr_align);
            
            addr += addr_align;
            buf += addr_align;
            
            word_num = (size - addr_align) >> 2;
            word_align_size = (size - addr_align) - ALIGN_DOWN((size - addr_align), 4);
            
            buff_align = ALIGN_UP((uint32_t)buf, 4) - (uint32_t)buf;
        }
        /* buff �����ֶ��� */
        if (buff_align == 0)
        {
            /* һ�ο������ */
            SFC_Read(addr, (uint32_t *)buf, word_num);
            addr += size - word_align_size;
            buf += size - word_align_size;
        }
        else /* buff �������ֶ��� */
        {
            /* ���ֶ��뿽�� */
            for (size_t i = 0; i < word_num; i++, buf += 4, addr += 4)
            {
                SFC_Read(addr, temp, 1);
                memcpy(buf, temp, sizeof(temp));
            }
        }
    }
    
    /* �������һ���ֵĲ��� */
    if (word_align_size != 0)
    {
        SFC_Read(addr, temp, 1);
        memcpy(buf, temp, word_align_size);
    }
    
    return size;
}

#define IOSPI_CS_Low()		GPIO_ClrBit(GPIOD, PIN6); __NOP(); __NOP(); __NOP(); __NOP()
#define IOSPI_CS_High()		__NOP(); __NOP(); __NOP(); __NOP(); GPIO_SetBit(GPIOD, PIN6)
#define IOSPI_CLK_Low()		GPIO_ClrBit(GPIOD, PIN5); __NOP(); __NOP()
#define IOSPI_CLK_High()	__NOP(); __NOP(); GPIO_SetBit(GPIOD, PIN5)
#define IOSPI_MOSI_Low()	GPIO_ClrBit(GPIOD, PIN8)
#define IOSPI_MOSI_High()	GPIO_SetBit(GPIOD, PIN8)
#define IOSPI_MISO_Value()	GPIO_GetBit(GPIOD, PIN7)

static uint8_t IOSPI_ReadWrite(uint8_t data)
{
	uint8_t val = 0;
	
	for(uint8_t i = 0; i < 8; i++)
	{
		IOSPI_CLK_Low();
		
		if(data & (1 << (7 - i)))
			IOSPI_MOSI_High();
		else
			IOSPI_MOSI_Low();
		
		IOSPI_CLK_High();
		
		val = (val << 1) | IOSPI_MISO_Value();
	}
	
	return val;
}

/****************************************************************************************************************************************** 
* ��������:	SFC_GPIOWrite_Bytes()
* ����˵��:	SFC д�����������д��ʱ�������� GPIO ģ�� SPI д��
* ��    ��: uint32_t addr		����Ҫд�뵽Flash�еĵ�ַ
*			uint8_t buff[]		Ҫд��Flash�е�����
*			uint32_t cnt		Ҫд�����ݵĸ��������ֽ�Ϊ��λ�����256
* ��    ��: ��
* ע������: ִ�д˺���ǰ��Ҫ����Ӧ�����е� GPIO ���ܣ�ʹ������ٴν���Ӧ�����л��� SFC ���ܣ��Ա�ʹ�� SFC ��������ȡ����
******************************************************************************************************************************************/
static void SFC_GPIOWrite_Bytes(uint32_t addr, const uint8_t *buff, uint32_t cnt)
{
	IOSPI_CS_Low();
	IOSPI_ReadWrite(SFC_CMD_WRITE_ENABLE);
	IOSPI_CS_High();
	
	IOSPI_CS_Low();
	IOSPI_ReadWrite(SFC_CMD_PAGE_PROGRAM);
	IOSPI_ReadWrite(addr >> 16);
	IOSPI_ReadWrite(addr >>  8);
	IOSPI_ReadWrite(addr);
	
	for(uint32_t i = 0; i < cnt; i++)
	{
		IOSPI_ReadWrite(buff[i]);
	}
	IOSPI_CS_High();
	
	int busy;
	do {
		IOSPI_CS_Low();
		IOSPI_ReadWrite(SFC_CMD_READ_STATUS_REG1);
		busy = IOSPI_ReadWrite(0xFF) & (1 << SFC_STATUS_REG_BUSY_Pos);
		IOSPI_CS_High();
	} while(busy);
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    if (size == 0) 
        return 0;
    
    long addr = nor_flash0.addr + offset;
    
    GPIO_INIT(GPIOD, PIN5, GPIO_OUTPUT);
    GPIO_INIT(GPIOD, PIN6, GPIO_OUTPUT);
    GPIO_INIT(GPIOD, PIN8, GPIO_OUTPUT);
    GPIO_INIT(GPIOD, PIN7, GPIO_INPUT_PullUp);
//    GPIO_INIT(GPIOD, PIN3, GPIO_INPUT_PullUp);
//    GPIO_INIT(GPIOD, PIN4, GPIO_INPUT_PullUp);

    size_t page_num = size >> 8;
    size_t page_align = ALIGN_UP(addr, 256) - addr;
    size_t size_align = size - ALIGN_DOWN(size, 256);
    
    /* д����������������һҳ������ */
    if (page_align != 0 && size >= page_align)
    {
        SFC_GPIOWrite_Bytes(addr, buf, page_align);
        addr += page_align;
        buf += page_align;
        
        page_num = (size - page_align) >> 8;
        size_align = (size - page_align) - ALIGN_DOWN((size - page_align), 256);
    }
    
    /* ��ҳ����д */
    for (size_t i = 0; i < page_num; i++, addr += 256, buf += 256)
    {
        SFC_GPIOWrite_Bytes(addr, buf, 256);
    }
    
    /* ���д�겻��һҳ�Ĳ��� */
    if (size_align != 0)
    {
        SFC_GPIOWrite_Bytes(addr, buf, size_align);
    }
    
    PORT_Init(PORTD, PIN5, PORTD_PIN5_FSPI_SCLK, 0);
    PORT_Init(PORTD, PIN6, PORTD_PIN6_FSPI_SSEL, 0);
    PORT_Init(PORTD, PIN8, PORTD_PIN8_FSPI_MOSI, 1);
    PORT_Init(PORTD, PIN7, PORTD_PIN7_FSPI_MISO, 1);
//    PORT_Init(PORTD, PIN3, PORTD_PIN3_FSPI_DATA2, 1);
//    PORT_Init(PORTD, PIN4, PORTD_PIN4_FSPI_DATA3, 1);
    
    return size;
}

static int erase(long offset, size_t size)
{
    if (size >= nor_flash0.len)
    {
        SFC_EraseEx(0xFFFFFFFF, SFC_CMD_ERASE_CHIP, 1);
        return size;
    }
    
    long addr = nor_flash0.addr + offset;
    uint8_t erase_cmd = 0;
    size_t block_size = 0;
    
    /* block Խ��, ����Ч��Խ�� */
    if (IS_ALIGN(addr, 64 * 1024) && IS_ALIGN(size, 64 * 1024))
    {
        erase_cmd = SFC_CMD_ERASE_BLOCK64KB;
        block_size = 64 * 1024;
    }
    else if (IS_ALIGN(addr, 32 * 1024) && IS_ALIGN(size, 32 * 1024))
    {
        erase_cmd = SFC_CMD_ERASE_BLOCK32KB;
        block_size = 32 * 1024;
    }
    else //if (IS_ALIGN(addr, 4 * 1024) && IS_ALIGN(size, 4 * 1024))
    {
        erase_cmd = SFC_CMD_ERASE_SECTOR;
        block_size = 4 * 1024;
        
        addr = ALIGN_DOWN(addr, block_size);
    }
    
    for (size_t i = 0; i < size; i += block_size, addr += block_size)
    {
        SFC_EraseEx(addr, erase_cmd, 1);
    }
    
    return size;
}

const struct fal_flash_dev nor_flash0 =
{
    .name       = "norflash0",
    .addr       = 0,
    .len        = 16 * 1024 * 1024,
    .blk_size   = 4 * 1024,
    .ops        = {init, read, write, erase},
    .write_gran = 8 //32 // SFC д����Ϊ 32bit ����, IO ģ��дΪ 8bit ����
};

