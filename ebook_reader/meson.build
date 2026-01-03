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
lvgl_dep = dependency('lvgl',
  fallback: ['lvgl', 'lvgl_dep'],
  required: true,
  static: true,
  default_options: [
     'CONFIG_LV_BUILD_EXAMPLES=false',
     'CONFIG_LV_BUILD_DEMOS=false',
     'werror=false',
  ],
)

ebook_reader_deps = [
   lvgl_dep,
]

if get_option('display') == 'x11'
   ebook_reader_deps += [dependency('x11', required: true)]
   add_project_arguments('-DEBOOK_READER_X11_ENABLED', language: 'c')   
endif

if get_option('display') == 'waveshare7in5v2b'
   ebook_reader_deps += [dependency('display_driver',
                                    fallback: ['display_driver', 'display_driver_dep'],
				    required: true,
				    static: true,
				    )]
   add_project_arguments('-DEBOOK_READER_WVS7IN5V2B_ENABLED', language: 'c')   
endif


ebook_reader_inc = include_directories(['src'])

ebook_reader_src = files('src/main.c',
                         'src/utils/error.c',			 
			 'src/gui/gui.c',
 			 'src/gui/lv/lv.c',
			 'src/gui/lv/display.c',
 			 'src/gui/view/view.c',
			 )

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
