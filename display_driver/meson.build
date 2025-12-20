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

display_driver_src = files('src/main.c',
	    'src/gpio.c',
	    'src/utils/err.c',
)

display_driver_deps = [
   libgpiod_dep,
]

exe = executable('display_driver',
  sources: display_driver_src,
  include_directories: display_driver_inc,
  c_args: ['-lgpiod'],
  link_args: ['-lgpiod'],
  dependencies: display_driver_deps,
)


# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif
