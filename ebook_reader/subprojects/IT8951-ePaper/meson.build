# IT8951-ePaper
# ******************************************************************************
# *    Project
# ******************************************************************************
project('it8951_epaper', 'c',
         version: '0.0.1',
         meson_version: '>=1.1',
         default_options: [
            'werror=true',
            'c_std=gnu99',
         ],
)

# ******************************************************************************
# *    Config
# ******************************************************************************
it8951_epaper_deps = []

cc = meson.get_compiler('c')
it8951_epaper_deps += [dependency('libgpiod',
                        required: true,
                       ),
                       cc.find_library('m', required: true),
]

it8951_epaper_inc = include_directories(['Raspberry/lib/e-Paper'])

it8951_epaper_src = [files('Raspberry/lib/e-Paper/EPD_IT8951.c',
			   'Raspberry/lib/Fonts/font12.c',
			   'Raspberry/lib/Fonts/font20.c',
			   'Raspberry/lib/Fonts/font16.c',
			   'Raspberry/lib/Fonts/font24.c',
			   'Raspberry/lib/Fonts/font12CN.c',
			   'Raspberry/lib/Fonts/font8.c',
			   'Raspberry/lib/Fonts/font24CN.c',
			   'Raspberry/lib/Config/DEV_Config.c',
			   'Raspberry/lib/Config/dev_hardware_SPI.c',
			   'Raspberry/lib/GUI/GUI_Paint.c',
			   'Raspberry/lib/GUI/GUI_BMPfile.c',
			   'Raspberry/examples/example.c',
                           # Add more files here
                         )
]

board = get_option('board')
if board == 'stm32'
   add_project_arguments('-DSTM32', language: 'c')
   it8951_epaper_src += [files('Raspberry/lib/Config/STM_gpiod.c')]
endif
if board == 'rpi'
   add_project_arguments('-DRPI', language: 'c')		
   it8951_epaper_src += [files('Raspberry/lib/Config/RPI_gpiod.c')]
endif

buildtype = get_option('buildtype')
if buildtype == 'debug' or buildtype == 'debugoptimized'
  add_project_arguments('-DDEBUG', language: 'c')
endif

add_project_arguments('-DGPIOD', language: 'c')
add_project_arguments('-DEBK', language: 'c')

# ******************************************************************************
# *    Lib
# ******************************************************************************
it8951_epaper_lib = static_library('it8951_epaper',
     sources: it8951_epaper_src,
     include_directories: it8951_epaper_inc,
     dependencies: it8951_epaper_deps,
)

it8951_epaper_dep = declare_dependency(
  link_with: it8951_epaper_lib,
  include_directories: it8951_epaper_inc,
  dependencies: it8951_epaper_deps,
)

# ******************************************************************************
# *    Exec
# ******************************************************************************
executable('it8951_refresh',
  sources: ['Raspberry/examples/refresh.c'],
  include_directories: it8951_epaper_inc,
  dependencies: it8951_epaper_deps + [it8951_epaper_dep],
)

executable('it8951_main',
  sources: ['Raspberry/examples/main.c'],
  include_directories: it8951_epaper_inc,
  dependencies: it8951_epaper_deps + [it8951_epaper_dep],
)
