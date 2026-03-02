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

add_global_arguments('-DGPIOD', language: ['c', 'cpp'])
cc = meson.get_compiler('c')

# add_global_arguments('-D_DEFAULT_SOURCE', language: ['c', 'cpp'])
it8951_epaper_deps += [dependency('libgpiod',
                        required: true,
                       ),
                       cc.find_library('m', required: true),
                       dependency('threads'),
]



# it8951_epaper_inc = include_directories(['src'])

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
			   # 'Raspberry/examples/main.c',
			   'Raspberry/examples/main2.c',
			   'Raspberry/examples/example.c',
			   # 'Raspberry/examples/main3.c',			   
                           # Add more files here
                         )
]

board = get_option('board')
if board == 'stm32'
   add_project_arguments('-DSTM32', language: 'c')
   add_project_arguments('-DRPI', language: 'c')	
   it8951_epaper_src += [files('Raspberry/lib/Config/STM_gpiod.c')]
else
   add_project_arguments('-DRPI', language: 'c')
   it8951_epaper_src += [files('Raspberry/lib/Config/RPI_gpiod.c')]
endif

add_project_arguments('-DGPIOD', language: 'c')		

buildtype = get_option('buildtype')
if buildtype == 'debug' or buildtype == 'debugoptimized'
  add_project_arguments('-DDEBUG', language: 'c')
endif

# ******************************************************************************
# *    Exec
# ******************************************************************************
it8951_epaper_exe = executable('it8951_epaper',
  sources: it8951_epaper_src,
  # include_directories: it8951_epaper_inc,
  dependencies: it8951_epaper_deps,
)
