
def GetCPPPATH(BSP_ROOT, RTT_ROOT):
	CPPPATH=[
		BSP_ROOT + "/.",
		BSP_ROOT + "/applications",
		BSP_ROOT + "/board",
		BSP_ROOT + "/libraries/GD32F4xx_Firmware_Library/CMSIS",
		BSP_ROOT + "/libraries/GD32F4xx_Firmware_Library/CMSIS/GD/GD32F4xx/Include",
		BSP_ROOT + "/libraries/GD32F4xx_Firmware_Library/GD32F4xx_standard_peripheral/Include",
		BSP_ROOT + "/libraries/gd32_drivers",
		BSP_ROOT + "/packages/CmBacktrace-v1.4.1",
		RTT_ROOT + "/components/dfs/dfs_v1/filesystems/devfs",
		RTT_ROOT + "/components/dfs/dfs_v1/filesystems/elmfat",
		RTT_ROOT + "/components/dfs/dfs_v1/filesystems/ramfs",
		RTT_ROOT + "/components/dfs/dfs_v1/include",
		RTT_ROOT + "/components/drivers/include",
		RTT_ROOT + "/components/drivers/spi",
		RTT_ROOT + "/components/drivers/spi/sfud/inc",
		RTT_ROOT + "/components/finsh",
		RTT_ROOT + "/components/legacy",
		RTT_ROOT + "/components/legacy/dfs",
		RTT_ROOT + "/components/libc/compilers/common/include",
		RTT_ROOT + "/components/libc/compilers/newlib",
		RTT_ROOT + "/components/libc/posix/io/poll",
		RTT_ROOT + "/components/libc/posix/io/stdio",
		RTT_ROOT + "/components/libc/posix/ipc",
		RTT_ROOT + "/components/libc/posix/libdl",
		RTT_ROOT + "/components/utilities/ymodem",
		RTT_ROOT + "/include",
		RTT_ROOT + "/libcpu/arm/common",
		RTT_ROOT + "/libcpu/arm/cortex-m4",
	]

	return CPPPATH

def GetCPPDEFINES():
	CPPDEFINES=['_POSIX_C_SOURCE=1', 'USE_STDPERIPH_DRIVER', 'RT_USING_LIBC', 'RT_USING_NEWLIBC', 'GD32F450', '__RTTHREAD__']
	return CPPDEFINES

