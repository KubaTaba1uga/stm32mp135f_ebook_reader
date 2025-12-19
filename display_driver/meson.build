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
src = files('src/main.c',
            'src/error.c',
	    'src/gpio.c',
	    'src/board.c',
	    'src/stm32board.c',
	    'src/utils/mem.c',
)

exe = executable('display_driver',
  sources: src,
  include_directories: display_driver_inc,
  c_args: ['-lgpiod'],
  link_args: ['-lgpiod'],  
)


# ******************************************************************************
# *    Tests
# ******************************************************************************
# if get_option('tests')
#    subdir('test')
# endif

# ******************************************************************************
# *    Example
# ******************************************************************************
# if get_option('examples')
#     subdir('example')
# endif
