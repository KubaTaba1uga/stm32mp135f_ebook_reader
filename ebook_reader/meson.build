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
   ebook_reader_deps += [dependency('x11', required: true)]
   add_global_arguments('-DEBK_DISPLAY_X11=1', language: ['c', 'cpp'])
elif get_option('display') == 'wvs7in5v2b'
   add_global_arguments('-DEBK_DISPLAY_WVS7IN5V2B=1', language: ['c', 'cpp'])
elif get_option('display') == 'wvs7in5v2'
   add_global_arguments('-DEBK_DISPLAY_WVS7IN5V2=1', language: ['c', 'cpp'])
endif

# Beautify logs, without these directives every log start with `../../`
add_global_arguments('-ffile-prefix-map=../../=', language: ['c', 'cpp'])
add_global_arguments('-fmacro-prefix-map=../../=', language: ['c', 'cpp'])

# ******************************************************************************
# *    Libraries
# ******************************************************************************
ebook_reader_deps += [dependency('display_driver',
                       fallback: ['display_driver', 'display_driver_dep'],
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
                       ]),
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
                      # dependency('sqlite3',
                      #  required: true,
                      # ),                       
]

ebook_reader_inc = include_directories(['src'])

ebook_reader_src = [files('src/main.c',
                          'src/app/core.c',
                          'src/app/module.c',                          
                          'src/app/module_menu.c',
                          'src/app/module_reader.c',
                          'src/app/module_error.c',
                          'src/ui/core.c',
                          'src/ui/widgets.c',
                          'src/ui/screen.c',
                          'src/ui/screen_menu.c',                                                    
                          'src/ui/display.c',
                          'src/ui/display_x11.c',
                          'src/ui/display_wvs7in5v2.c',			  
                          'src/ui/display_wvs7in5v2b.c',
                          'src/book/core.c',
                          'src/book/pdf.c',
                          'src/utils/err.c',
                          'src/utils/mem.c',
                          'src/utils/settings.c',
                          'src/utils/time.c',
                          'src/utils/zlist.c',
                          'src/utils/graphic.c',
                          # Add more files here
                         )
]

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

pkgdatadir = 'assets'
install_subdir('data', install_dir: pkgdatadir)

# ******************************************************************************
# *    Tests
# ******************************************************************************
if get_option('tests')
   subdir('test')
endif
