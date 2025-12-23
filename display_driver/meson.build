# Display driver
# ******************************************************************************
# *    Project
# ******************************************************************************
project('display_driver', 'c',
         version: '0.0.1',
	 meson_version: '>=1.1',
	 default_options: [
            'werror=true',
            'c_std=c11',
         ],
)

# ******************************************************************************
# *    Library
# ******************************************************************************
display_driver_inc = include_directories(['include', 'src'])

libgpiod_dep = dependency('libgpiod',
  fallback: ['libgpiod', 'gpiod_dep'],
  required: true,
  static: true,
)

display_driver_src = [files(
			   'src/gpio.c',
			   'src/spi.c',			   
			   'src/boards/board.c',
			   'src/boards/stm32mp135f_dk.c',
			   'src/boards/raspberrypi_5.c',	    			   
			   'src/utils/err.c',
			   'src/utils/time.c',			   
)]

display_driver_deps = [
   libgpiod_dep,
]

exe = executable('dd_stm32',
  sources: display_driver_src + [files('src/stm32.c')],
  include_directories: display_driver_inc,
  dependencies: display_driver_deps,
)

exe = executable('dd_rpi',
  sources: display_driver_src + [files('src/rpi.c')],
  include_directories: display_driver_inc,
  dependencies: display_driver_deps,
)

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif
