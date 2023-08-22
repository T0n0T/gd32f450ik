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
#include "dfs_fs.h"

#define LOG_TAG "drv.fs"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define USE_NORFLASH  0
#define USE_NANDFLASH 1

#if USE_NORFLASH

#include "spi_flash.h"
#include "spi_flash_sfud.h"
#include "drv_spi.h"

static int file_system_format(void)
{
    // 初始化，格式化flash
    return dfs_mkfs("elm", "gd25q40");
}
MSH_CMD_EXPORT(file_system_format, format filesystem);

static int rt_hw_file_system_mount(void)
{
    return dfs_mount("gd25q40", "/", "elm", 0, 0); // 注册块设备，这一步可以将外部flash抽象为系统的块设备
}
INIT_ENV_EXPORT(rt_hw_file_system_mount);
#endif

#if USE_NANDFLASH
#include <fal.h>

#define FS_PARTITION_NAME NAND_FLASH_PARTION_NAME
static int file_system_format(void)
{
    // 初始化，格式化flash
    return dfs_mkfs("elm", FS_PARTITION_NAME);
}
MSH_CMD_EXPORT(file_system_format, format filesystem);

static int rt_hw_file_system_mount(void)
{
    fal_init();

    struct rt_device *flash_dev = fal_blk_device_create(FS_PARTITION_NAME);
    if (flash_dev == NULL) {
        LOG_E("Can't create a block device on '%s' partition.", FS_PARTITION_NAME);
        return -1;
    } else {
        LOG_D("Create a block device on the %s partition of flash successful.", FS_PARTITION_NAME);
    }
    // 注册块设备，这一步可以将外部flash抽象为系统的块设备
    dfs_mkfs("elm", FS_PARTITION_NAME);
    if (dfs_mount(FS_PARTITION_NAME, "/", "elm", 0, 0) == RT_EOK) {
        LOG_D("elm filesystem mount to '/'");
    } else {
        LOG_E("elm filesystem mount to '/' failed!");
        return -1;
    }
    return 0;
}
INIT_ENV_EXPORT(rt_hw_file_system_mount);

#endif