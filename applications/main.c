/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-20     BruceOu      first implementation
 */

#include <stdio.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LED1_PIN GET_PIN(G, 3)

/* SDRAM */
#define BUFFER_SIZE ((uint32_t)0x0400)
#define WRITE_READ_ADDR ((uint32_t)0x0000)
uint32_t writereadstatus = 0;
uint8_t txbuffer[BUFFER_SIZE];
uint8_t rxbuffer[BUFFER_SIZE];
#define delay_1ms rt_thread_mdelay

void cal(int argc, char *argv[])
{
    if (argc < 1)
    {
        return;
    }
    rt_int8_t a = strtoul(argv[1], NULL, 10);
    rt_int8_t b = strtoul(argv[2], NULL, 10);
    rt_uint8_t c = strtoul(argv[1], NULL, 10);
    rt_uint8_t d = strtoul(argv[2], NULL, 10);
    printf("char: %d\n", a - b);
    printf("uchar: %d\n", c - d);
    printf("uint8: 0x%X\n", c - d);
}
MSH_CMD_EXPORT(cal, calculation);

void sdram()
{
    printf("\r\nSDRAM initialized!");
    delay_1ms(1000);

    /* fill txbuffer */
    fill_buffer(txbuffer, BUFFER_SIZE, 0x0000);
    sdram_writebuffer_8(EXMC_SDRAM_DEVICE0, txbuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    printf("\r\nSDRAM write data completed!");
    delay_1ms(1000);

    /* read data from SDRAM */
    sdram_readbuffer_8(EXMC_SDRAM_DEVICE0, rxbuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    printf("\r\nSDRAM read data completed!");
    delay_1ms(1000);

    printf("\r\nCheck the data!");
    delay_1ms(1000);

    /* compare two buffers */
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (rxbuffer[i] != txbuffer[i])
        {
            writereadstatus++;
            break;
        }
    }

    if (writereadstatus)
    {
        printf("\r\nSDRAM test failed!");
    }
    else
    {
        printf("\r\nSDRAM test successed!");
        delay_1ms(1000);
        printf("\r\nThe data is:\r\n");
        delay_1ms(1000);
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            printf("%6x", rxbuffer[i]);
            if (((i + 1) % 16) == 0)
            {
                printf("\r\n");
            }
        }
    }
}
MSH_CMD_EXPORT(sdram, test sdram);

struct rt_memheap sdram_heap;

int main(void)
{
    rt_ubase_t begin_align = RT_ALIGN((rt_ubase_t)EXT_SDRAM_BEGIN, RT_ALIGN_SIZE);
    rt_ubase_t end_align   = RT_ALIGN_DOWN((rt_ubase_t)EXT_SDRAM_END, RT_ALIGN_SIZE);
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);

//    rt_memheap_init(&sdram_heap, "sdram", (void*)EXT_SDRAM_BEGIN, end_align-begin_align);
    printf("start_addr: 0x%08x size: %d\n", EXT_SDRAM_BEGIN, end_align-begin_align);
    while (1)
    {
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED1_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}
