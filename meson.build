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
  '-DGD32F450',
  '-DGD32F450I_EVAL',
  '-mcpu=cortex-m4',
  '-mthumb',
  '-mfpu=fpv4-sp-d16',
  '-mfloat-abi=hard',    
  '-Wall',
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
  '-nostdlib',
  '-Xlinker',
  '-Map=' + meson.current_build_dir() + '/baselayer.map',
  '-T', meson.current_source_dir() + '/gd32f4xx.ld',
  '-Wl,--gc-sections',     # 删除未使用的段
  '-Wl,--print-memory-usage', # 打印内存使用情况
]

# 包含子目录构建脚本
subdir('firmware')
subdir('application')
subdir('platform')
subdir('board')
subdir('middlewares/cm_backtrace')
subdir('middlewares/printf')
subdir('demo')

appdep_incdirs = [ printf_incs ]
appdep_sources = [ printf_srcs ]

if get_option('buildtype').startswith('debug')
  message('Use debug buildtype' + get_option('buildtype'))
  appdep_sources += cmb_srcs
  appdep_incdirs += cmb_incs
  c_args += '-DCM_BACKTRACE_ENABLE'  # 添加编译宏以启用相关代码
  c_args += '-DCMB_USER_CFG'
  c_args += '-DDEBUG'
endif

# 创建最终可执行文件
baselayer_elf = executable('baselayer',
  sources: [ 
    applications_sources,
    firmware_sources,
    platform_sources, 
    board_sources,
    appdep_sources,
  ],
  include_directories: [
    applications_incdirs,
    firmware_incdirs, 
    platform_incdirs,
    board_incdirs,
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
  input: baselayer_elf,
  output: 'baselayer.hex',
  command: [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
  build_by_default: true
)

run_target('disasm',
  command: [
    'sh', '-c',
    objdump.full_path() + ' -d -l -C ' + baselayer_elf.full_path() + ' > ' + 
    meson.current_build_dir() / 'baselayer.dis'
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


# 编译 demo.elf
demo_elf = executable('demo',
  sources: demo_sources,
  include_directories: [
    demo_incdirs,
    platform_incdirs, # 包含 platform 目录的头文件
  ],
  c_args: demo_c_args,
  link_args: demo_link_args,
  name_suffix: 'elf'
)

custom_target('demo.hex',
  input: demo_elf,
  output: 'demo.hex',
  command: [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
)

# 生成 demo.dis
run_target('demo.dis',
  command: [
    'sh', '-c',
    objdump.full_path() + ' -d -l -C ' + demo_elf.full_path() + ' > ' +
    meson.current_build_dir() / 'demo.dis'
  ]
)
