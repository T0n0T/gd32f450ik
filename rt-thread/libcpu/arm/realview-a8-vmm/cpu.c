/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-09-15     Bernard      first version
 */

#include <rthw.h>
#include <rtthread.h>
#include <board.h>

/**
 * @addtogroup AM33xx
 */
/*@{*/

/** shutdown CPU */
rt_weak void rt_hw_cpu_shutdown()
{
    rt_base_t level;
    rt_kprintf("shutdown...\n");

    level = rt_hw_interrupt_disable();
    while (level)
    {
        RT_ASSERT(0);
    }
}

/*@}*/
