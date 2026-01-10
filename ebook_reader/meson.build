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
# *    Config
# ******************************************************************************
ebook_reader_deps = []
add_global_arguments('-D_DEFAULT_SOURCE', language: ['c', 'cpp'])
if get_option('display') == 'x11'
   add_global_arguments('-DEBK_DISPLAY_X11=1', language: ['c', 'cpp'])
   ebook_reader_deps += [dependency('x11', required: true)]
   # run_command(import('python').find_installation('python3'), files('tools/render_lv_conf.py'), 'x11=1', check: true)
elif get_option('display') == 'wvs7in5v2b'
   add_global_arguments('-DEBK_DISPLAY_WVS7IN5V2B=1', language: ['c', 'cpp'])
   # run_command(import('python').find_installation('python3'), files('tools/render_lv_conf.py'), 'wvs7in5v2b=1', check: true)
endif

# ******************************************************************************
# *    Libraries
# ******************************************************************************
ebook_reader_deps += [dependency('display_driver',
		      fallback: ['display_driver', 'display_driver_dep'],
		      required: true,
		      static: true,
		     ),
		     dependency('sqlite3',
		      fallback: ['sqlite3'],
		      required: true,
		      static: true,
		     ),
		     dependency('lvgl',
		       fallback: ['lvgl', 'lvgl_dep'],
		       required: true,
		       static: true,
		       default_options: [
			  'CONFIG_LV_BUILD_EXAMPLES=false',
			  'CONFIG_LV_BUILD_DEMOS=false',
			  'werror=false',
		       ],
                     ),		     
]

ebook_reader_inc = include_directories(['src'])

ebook_reader_src = [files('src/main.c',
			  'src/utils/error.c',
			  'src/utils/mem.c',
			  'src/utils/settings.c',
			  'src/utils/time.c',
			  'src/utils/lvgl.c',			  
			  'src/gui/gui.c',
			  'src/display/display.c',
			  'src/display/wvs7in5v2b.c',
			  'src/display/x11.c',			  			  
			  'src/core/core.c',
			  'src/core/menu.c',
			  'src/core/book_reader.c',
			  'src/core/error.c',			  
			 )]

buildtype = get_option('buildtype')
if buildtype == 'debug' or buildtype == 'debugoptimized'
  add_project_arguments('-DDEBUG', language: 'c')
endif

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
