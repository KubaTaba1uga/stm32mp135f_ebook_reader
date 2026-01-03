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
	    'default_library=static',
         ],
)

# ******************************************************************************
# *    Library
# ******************************************************************************
libgpiod_dep = dependency('libgpiod',
  fallback: ['libgpiod', 'gpiod_dep'],
  required: true,
  static: true,
)

display_driver_inc = include_directories(['include', 'src'])

display_driver_src = [files('src/image.c',
                            'src/waveshare_7in5_V2b.c',
		            'src/gpio/gpio.c',
		            'src/spi/spi.c',		    
			    'src/utils/err.c',
			    'src/utils/time.c',
)]

display_driver_deps = [
   libgpiod_dep,
]

display_driver_lib = library('display_driver',
     sources: display_driver_src,
     include_directories: display_driver_inc,
     dependencies: display_driver_deps,
)

display_driver_dep = declare_dependency(
  link_with: display_driver_lib,
  include_directories: display_driver_inc,
)

# ******************************************************************************
# *    Examples
# ******************************************************************************
if get_option('examples') == 'rpi'
   rpi_rst_example = executable('rpi_rst_example',
     sources: display_driver_src + [files('examples/rpi4b/rpi_rst_example.c')],
     include_directories: display_driver_inc,
     dependencies: display_driver_dep,
   )

   rpi_clear_example = executable('rpi_clear_example',
     sources: display_driver_src + [files('examples/rpi4b/rpi_clear_example.c')],
     include_directories: display_driver_inc,
     dependencies: display_driver_dep,
   )

   rpi_full_example = executable('rpi_display_full_example',
     sources: display_driver_src + [files('examples/rpi4b/rpi_display_full_example.c')],
     include_directories: display_driver_inc,
     dependencies: display_driver_dep,
   )
endif


if get_option('examples') == 'stm'
   stm_rst_example = executable('stm_rst_example',
     sources: display_driver_src + [files('examples/stm32mp135f/stm_rst_example.c')],
     include_directories: display_driver_inc,
     dependencies: display_driver_dep,
   )

   stm_clear_example = executable('stm_clear_example',
     sources: display_driver_src + [files('examples/stm32mp135f/stm_clear_example.c')],
     include_directories: display_driver_inc,
     dependencies: display_driver_dep,
   )

   stm_full_example = executable('stm_display_full_example',
     sources: display_driver_src + [files('examples/stm32mp135f/stm_display_full_example.c')],
     include_directories: display_driver_inc,
     dependencies: display_driver_dep,
   )
endif

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif
