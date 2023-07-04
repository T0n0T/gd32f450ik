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
#include <drv_exmc_sdram.h>
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define LED1_PIN GET_PIN(G, 3)

struct rt_memheap sdram_heap;
struct rt_memheap extra_heap;
int sdram_heap_init()
{
    rt_err_t err = RT_EOK;
    err = rt_memheap_init(&sdram_heap, "sdram", (void *)EXT_SDRAM_BEGIN, EXT_SDRAM_CAP);
    if (err != RT_EOK)
    {
        printf("sdram init heap failed\n");
        return -RT_ERROR;
    }
    err = rt_memheap_init(&extra_heap, "extra", (void *)0x10000000, 0x10000);
    if (err != RT_EOK)
    {
        printf("extra init heap failed\n");
        return -RT_ERROR;
    }
}
INIT_BOARD_EXPORT(sdram_heap_init);

void test(void* p)
{
    int count = 0;
    rt_thread_t thread = rt_thread_self();
    while(1)
    {
        count++;
        printf("%d %p\n", count, count);
        printf("%s sp --> 0x%08x \n", thread->parent.name, thread->sp);

        printf_register_in_thread(thread);

        rt_thread_mdelay(500);
    }

}

int main(void)
{
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);

    rt_thread_t tt = RT_NULL;
    tt = rt_thread_create("try", test, RT_NULL, 666, 20, 15);
    rt_thread_startup(tt);
    while (1)
    {
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED1_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}
