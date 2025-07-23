project('gd32boot_bin', 'c',
  version: '1.0.0',
  default_options: [
    'buildtype=debugoptimized',
    'c_std=gnu11',
  ],
  meson_version: '>=0.60.0')

# 设置交叉编译工具链配置文件
cross_file = 'tools/toolchains/arm-none-eabi-gcc.txt'

# 全局编译参数
c_args = [
  '-D__FPU_PRESENT',
  '-DGD32F450',
  '-DGD32F450I_EVAL',
  '-mcpu=cortex-m4',
  '-mthumb',
  '-mfpu=fpv4-sp-d16',
  '-mfloat-abi=hard',
  '-Wall',
  '-specs=nosys.specs',
  '-ffunction-sections',
  '-fdata-sections',
  '-O0',                    # 优化等级2
  '-fdata-sections',        # 将数据放入独立段
  '-ffunction-sections',    # 将函数放入独立段
]

# 全局链接参数
link_args = [
  '-mcpu=cortex-m4',
  '-mthumb',
  '-mfpu=fpv4-sp-d16',
  '-mfloat-abi=hard',
  '-Wl,--gc-sections',
  '-specs=nosys.specs',
  '-Xlinker',
  '-Map=' + meson.current_build_dir() + '/gd32f4xx.map',
  '-T', meson.current_source_dir() + '/gd32f4xx.ld',
  '-Wl,--gc-sections',     # 删除未使用的段
  '-Wl,--print-memory-usage', # 打印内存使用情况
]

# 包含子目录构建脚本
subdir('firmware')
subdir('application')
subdir('board')
subdir('middlewares/cm_backtrace')
subdir('middlewares/SEGGER')
subdir('middlewares/CherryUSB')

appdep_incdirs = [cherryusb_incs]
appdep_sources = [cherryusb_srcs]
if get_option('buildtype').startswith('debug')
  message('Use debug buildtype' + get_option('buildtype'))
  appdep_sources += cmb_srcs
  appdep_incdirs += cmb_incs
  c_args += '-DCM_BACKTRACE_ENABLE'  # 添加编译宏以启用相关代码
  c_args += '-DCMB_USER_CFG'
  c_args += '-DDEBUG'
  appdep_sources += segger_srcs
  appdep_incdirs += segger_incs
endif

c_args += '-DUSE_MODBUS'

# 创建最终可执行文件
gd32f4xx_elf = executable('gd32f4xx',
  sources: [ 
    applications_sources,
    board_sources,
    firmware_sources,
    appdep_sources,
  ],
  include_directories: [
    applications_incdirs,
    board_incdirs,
    firmware_incdirs, 
    appdep_incdirs,
  ],
  c_args: c_args,
  link_args: link_args,
  name_suffix: 'elf'
)

# 配置objcopy工具
objcopy = find_program('arm-none-eabi-objcopy')
objdump = find_program('arm-none-eabi-objdump')

# 根据操作系统选择JLink可执行文件名称
jlink_exe = 'JLinkExe'
if build_machine.system() == 'windows'
  jlink_exe = 'JLink'
endif

# 配置JLink工具
jlink = find_program(jlink_exe)

# 添加hex文件生成目标
custom_target('hex',
  input: gd32f4xx_elf,
  output: 'gd32f4xx.hex',
  command: [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
  build_by_default: true
)

run_target('disasm',
  command: [
    'sh', '-c',
    objdump.full_path() + ' -d -S -l -C ' + gd32f4xx_elf.full_path() + ' > ' + 
    meson.current_build_dir() / 'gd32f4xx.dis'
  ]
)

# 添加下载目标
if get_option('JLINK_REMOTE') != 'disabled'
  message('JLink remote debug enabled')
  run_target('flash',
    command: [
      jlink,
      '-ip', get_option('JLINK_REMOTE'),
      '-device', 'GD32F450IK',
      '-if', 'SWD',
      '-speed', '4000',
      '-autoconnect', '1',
      '-CommanderScript', meson.current_source_dir() + '/tools/flash.jlink'
    ]
  )
else
  run_target('flash',
    command: [
      jlink,
      '-device', 'GD32F450IK',
      '-if', 'SWD',
      '-speed', '4000',
      '-autoconnect', '1',
      '-CommanderScript', meson.current_source_dir() + '/tools/flash.jlink'
    ]
  )
endif

