/*
 * Copyright (c) 2024, Synwit, <linzh@synwit.cn>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fal.h>
#include <string.h>
#include "framework.h"

/* note: align ������ 2 ��ָ���� */

#define IS_ALIGN(val, align)        ( ( !( (val) & ( (align) - 1 ) ) ) ) //&& (val) )

#define ALIGN_UP(val, align)        ( ( (val) + (align) - 1 ) & ~( (align) - 1 ) )

#define ALIGN_DOWN(val, align)      ( (val) & ~( (align) - 1 ) )


static int init(void)
{
    /* do nothing now */
    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    long addr = onchip_flash.addr + offset;

    for (size_t i = 0; i < size; i++, buf++, addr++)
    {
        *buf = *(uint8_t *)addr;
    }

    return size;
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    if (size == 0)
        return 0;
    
    long addr = onchip_flash.addr + offset;
    
    if (!IS_ALIGN(addr, 16) || !IS_ALIGN(size, 16))
    {
        printf("[FAL] [ERROR]: addr = 0x[%lx], size = 0x[%x]\r\n", addr, size);
        return -1;
    }
    
    if (IS_ALIGN((uint32_t)buf, 4))
    {
        if (FLASH_RES_OK != FLASH_Write(addr, (uint32_t *)buf, size >> 2))
        {
            printf("[FAL] [ERROR]: addr = 0x[%lx], size = 0x[%x], buf = 0x[%p]\r\n", addr, size, buf);
            return -1;
        }
    }
    else 
    {
        uint32_t write_gran_buf[4] = {0};
        for (size_t i = 0; i < size / sizeof(write_gran_buf); )
        {
            memcpy(write_gran_buf, buf, sizeof(write_gran_buf));
            if (FLASH_RES_OK == FLASH_Write(addr, write_gran_buf, sizeof(write_gran_buf) >> 2))
            {
                addr += sizeof(write_gran_buf);
                buf += sizeof(write_gran_buf);
                ++i;
            }
        }
    }
    
    return size;
}

static int erase(long offset, size_t size)
{
    long addr = ALIGN_DOWN(onchip_flash.addr + offset, 4096);
    size = ALIGN_UP(size, 4096);
    
    for (size_t i = 0; i < size; i += 4096, addr += 4096)
    {
        FLASH_Erase(addr);
    }
    
    return size;
}

const struct fal_flash_dev onchip_flash =
{
    .name       = "onchip",
    .addr       = 0,
    .len        = 512 * 1024,
    .blk_size   = 4 * 1024,
    .ops        = {init, read, write, erase},
    .write_gran = 128
};

