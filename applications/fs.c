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
#include <drv_exmc_nandflash.h>
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

rt_device_t flash_dev = 0;
#define FS_PARTITION_NAME NAND_FLASH_PARTION_NAME
static int
file_system_format(void)
{
    // 初始化，格式化flash
    return dfs_mkfs("elm", FS_PARTITION_NAME);
}
MSH_CMD_EXPORT(file_system_format, format filesystem);

static int rt_hw_file_system_mount(void)
{
    fal_init();
    flash_dev = fal_blk_device_create(FS_PARTITION_NAME);
    if (flash_dev == NULL) {
        LOG_E("Can't create a block device on '%s' partition.", FS_PARTITION_NAME);
        return -1;
    } else {
        LOG_D("Create a block device on the %s partition of flash successful.", FS_PARTITION_NAME);
    }
    // 注册块设备，这一步可以将外部flash抽象为系统的块设备
    int mkfs_res = dfs_mkfs("elm", FS_PARTITION_NAME);
    if (mkfs_res != 0) {
        LOG_E("dfs_mkfs error, errno = %d", mkfs_res);
        return -1;
    }
    // rt_device_open(flash_dev, RT_DEVICE_OFLAG_RDWR);
    if (dfs_mount(FS_PARTITION_NAME, "/", "elm", 0, 0) == RT_EOK) {
        LOG_D("elm filesystem mount to '/'");
    } else {
        LOG_E("elm filesystem mount to '/' failed!");
        return -1;
    }
    return 0;
}
INIT_ENV_EXPORT(rt_hw_file_system_mount);

#define TEST_LEN 2048
uint8_t *test = 0, *test2 = 0;
void send_test(int argc, char const *argv[])
{
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    if (argc < 1) {
        return;
    }
    int secter_num = atoi(argv[1]);
    test = (uint8_t *)rt_calloc(1, TEST_LEN);
    for (size_t i = 0; i < TEST_LEN; i++) {
        *(test + i) = i;
        // printf("%02x", i);
        // if (i % 40 == 39) {
        //     printf("\n");
        // }
    }
    printf("\n=================send_test\n");
    // rt_device_write(flash_dev, 0, test, 1);
    nand_write(secter_num * 2048, test, TEST_LEN);
    rt_free(test);
}
MSH_CMD_EXPORT(send_test, test);

void recv_test(int argc, char const *argv[])
{
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    if (argc < 1) {
        return;
    }
    int secter_num = atoi(argv[1]);
    test2          = (uint8_t *)rt_malloc(2048);
    for (size_t i = 0; i < 2048; i++) {
        *(test2 + i) = 0xff;
    }
    // rt_device_read(flash_dev, 0, test2, 1);
    nand_read(secter_num * 2048, test2, 2048);
    for (size_t i = 0; i < 2048; i++) {
        printf("%02x", *(test2 + i));
        if (i % 64 == 63) {
            printf("\n");
        }
    }
    printf("\n=================check sector %d\n", secter_num);
    rt_free(test2);
}
MSH_CMD_EXPORT(recv_test, test);

void open_flash(void)
{
    if (!rt_device_open(flash_dev, RT_DEVICE_OFLAG_RDWR)) {
        printf("open device %s success!\n", flash_dev->parent.name);
    } else {
        printf("open device %s fail!\n", flash_dev->parent.name);
    }
}
MSH_CMD_EXPORT(open_flash, test);

void erase_test(void)
{
    nand_format();
}
MSH_CMD_EXPORT(erase_test, test);
#endif