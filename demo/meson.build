# demo 相关的源文件和头文件
demo_sources = files('demo.c')
demo_incdirs = include_directories('.')

# 根据 build.sh 定义的编译参数
demo_c_args = [
  '-mcpu=cortex-m4',
  '-mthumb',
  '-mfpu=fpv4-sp-d16',
  '-mfloat-abi=hard',
  '-g',
]

# 根据 build.sh 定义的链接参数
demo_link_args = [
  '-mcpu=cortex-m4',
  '-mthumb',
  '-mfpu=fpv4-sp-d16',
  '-mfloat-abi=hard',
  '-nostdlib',
  '-ffreestanding',
  '-T', meson.current_source_dir() / 'demo.ld',
  '-Wl,-Map=' + meson.current_build_dir() / 'demo.map',
]
