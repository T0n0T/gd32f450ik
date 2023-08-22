/**
 * @file fal_cfg.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-08-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <rtconfig.h>
#include <board.h>
#include <drv_exmc_nandflash.h>

#define NAND_FLASH_DEV_NAME     "nandflash"
#define NAND_FLASH_PARTION_NAME "nandfs"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev nand_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE \
    {                       \
        &nand_flash,        \
    }
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE               \
    {                                \
        {                            \
            FAL_PART_MAGIC_WORD,     \
            NAND_FLASH_PARTION_NAME, \
            NAND_FLASH_DEV_NAME,     \
            0,                       \
            (int)NAND_MAX_ADDRESS,   \
            0,                       \
        },                           \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */