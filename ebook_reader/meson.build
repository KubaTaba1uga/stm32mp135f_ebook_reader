# Ebook reader
# ******************************************************************************
# *    Project
# ******************************************************************************
project('ebook_reader', 'c',
         version: '0.0.1',
	 meson_version: '>=1.1',
	 default_options: [
            'werror=true',
            'c_std=c11',
         ],
)

# ******************************************************************************
# *    Libraries
# ******************************************************************************
# LV_BUILD_LVGL_H_SIMPLE_INCLUDE
lvgl_dep = dependency('lvgl',
  fallback: ['lvgl', 'lvgl_dep'],
  required: true,
  static: true,
  default_options: [
     # 'werror=false',
     # 'LV_BUILD_CONF_PATH="lv_conf.h"',
  ],
)

ebook_reader_deps = [
   lvgl_dep,
]

ebook_reader_inc = include_directories(['src', 'subprojects'])

ebook_reader_src = files('src/main.c')

# ******************************************************************************
# *    App
# ******************************************************************************
ebook_reader_exe = executable('ebook_reader',
  sources: ebook_reader_src,
  include_directories: ebook_reader_inc,
  dependencies: ebook_reader_deps,
)

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif
