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

# ******************************************************************************
# *    Examples
# ******************************************************************************
rpi_rst_example = executable('rpi_rst_example',
  sources: display_driver_src + [files('examples/rpi4b/rpi_rst_example.c')],
  include_directories: display_driver_inc,
  dependencies: display_driver_deps,
  # c_args: ['-D_GNU_SOURCE'],
)

rpi_clear_example = executable('rpi_clear_example',
  sources: display_driver_src + [files('examples/rpi4b/rpi_clear_example.c')],
  include_directories: display_driver_inc,
  dependencies: display_driver_deps,
  # c_args: ['-D_GNU_SOURCE'],
)

rpi_full_example = executable('rpi_display_full_example',
  sources: display_driver_src + [files('examples/rpi4b/rpi_display_full_example.c')],
  include_directories: display_driver_inc,
  dependencies: display_driver_deps,
  # c_args: ['-D_GNU_SOURCE'],
)

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif
