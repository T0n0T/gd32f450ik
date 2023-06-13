/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-11     liwentai       the first version
 */
#include <rtthread.h>
#include "spi_flash.h"
#include "spi_flash_sfud.h"
#include "drv_spi.h"
#include "dfs_fs.h"

static int rt_hw_spi_flash_init(void)
{
    rt_hw_spi_device_attach("spi1", "spi10", GET_PIN(C, 13));

    if (RT_NULL == rt_sfud_flash_probe("W25Q128", "spi10"))
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}
//INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);

static int rt_hw_spi_file_system_init(void)
{
    //初始化，格式化flash
    //dfs_mkfs("elm","W25Q128");
    if(dfs_mount("W25Q128", "/","elm",0,0) == 0)  //注册块设备，这一步可以将外部flash抽象为系统的块设备
    {
        return -RT_ERROR;
    }
    return RT_EOK;
}
//INIT_ENV_EXPORT(rt_hw_spi_file_system_init);


