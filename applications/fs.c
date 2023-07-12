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

static int file_system_format(void)
{
    //初始化，格式化flash
    return dfs_mkfs("elm","gd25q40");
}
MSH_CMD_EXPORT(file_system_format, format spi file system);

static int rt_hw_file_system_mount(void)
{
    return dfs_mount("gd25q40", "/","elm",0,0); //注册块设备，这一步可以将外部flash抽象为系统的块设备
}
INIT_ENV_EXPORT(rt_hw_file_system_mount);
