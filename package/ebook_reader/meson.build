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
add_global_arguments('-D_DEFAULT_SOURCE', language: ['c', 'cpp'])
add_global_arguments('-DLV_CONF_INCLUDE_SIMPLE', language: ['c', 'cpp'])

# Beautify logs, without these directives every log start with `../../`
add_global_arguments('-ffile-prefix-map=../../=', language: ['c', 'cpp'])
add_global_arguments('-fmacro-prefix-map=../../=', language: ['c', 'cpp'])

# ******************************************************************************
# *    Libraries
# ******************************************************************************
ebook_reader_deps = []
ebook_reader_src = []

buildtype = get_option('buildtype')
if buildtype == 'debug' or buildtype == 'debugoptimized'
  add_global_arguments('-DDEBUG', language: ['c', 'cpp'])
  add_global_arguments('-pg', language: ['c', 'cpp'])
endif

display = get_option('display')
if display == 'x11'
   add_global_arguments('-DEBK_DISPLAY_X11', language: ['c', 'cpp'])   
   ebook_reader_deps += [dependency('x11', required: true)]
   ebook_reader_src += [files('src/display/display_x11.c')]
elif display == 'png'
   add_global_arguments('-DEBK_DISPLAY_PNG', language: ['c', 'cpp'])   
   ebook_reader_src += [files('src/display/display_png.c')]   
else
   add_global_arguments('-DEBK_DISPLAY_IT8951', language: ['c', 'cpp'])      
   ebook_reader_deps += [dependency('it8951_epaper', required: true)]
   ebook_reader_src += [files('src/display/display_it8951.c')]
endif

ebook_reader_deps += [dependency('lvgl',
                        required: true,
		       ),
                       dependency('poppler',
                        required: true,
                       ),
                       dependency('glib-2.0',
                        required: true,
                       ),
                       dependency('poppler-glib',
                        required: true,
                       ),
                       dependency('cairo',
                        required: true,
                       ),
                       dependency('fontconfig',
                        required: true,
                       ),                       
                       dependency('sqlite3',
                        required: true,
                       ),
]

ebook_reader_inc = include_directories(['src'])

ebook_reader_src += [files('src/main.c',
                          'src/app/app.c',
                          'src/book_settings/book_settings.c',
                          'src/book_settings/view.c',
                          'src/book_settings/widgets.c',
	  		  'src/db/db.c',
	  		  'src/display/display.c',			  
                          'src/event_queue/event_queue.c',			  
                          'src/library/library.c',
                          'src/library/pdf.c',
                          'src/menu/menu.c',
                          'src/menu/view.c',
                          'src/menu/widgets.c',			  			  
                          'src/reader/reader.c',
                          'src/reader/view.c',
                          'src/reader/widgets.c',			  			  
                          'src/utils/err.c',
                          'src/utils/mem.c',
                          'src/utils/settings.c',
                          'src/utils/time.c',
                          'src/utils/zlist.c',
                          'src/utils/graphic.c',
                          'src/utils/lvgl.c',			  
                          # Add more files here
                         )
]



# ******************************************************************************
# *    Lib
# ******************************************************************************
ebook_reader_lib = static_library('ebook_reader',
     sources: ebook_reader_src,
     include_directories: ebook_reader_inc,
     dependencies: ebook_reader_deps,
)

ebook_reader_dep = declare_dependency(
  link_with: ebook_reader_lib,
  include_directories: ebook_reader_inc,
  dependencies: ebook_reader_deps,
)

# ******************************************************************************
# *    App
# ******************************************************************************
ebook_reader_exe = executable('ebook_reader',
  include_directories: ebook_reader_inc,
  dependencies: ebook_reader_deps + [ebook_reader_dep],
)

pkgdatadir = 'assets'
install_subdir('data', install_dir: pkgdatadir)

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif 

# ******************************************************************************
# *    Benchmarks
# ******************************************************************************
if get_option('benchmarks')
   subdir('benchmark')
endif
