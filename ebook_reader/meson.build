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
elif get_option('display') == 'wvs7in5v2b'
   add_global_arguments('-DEBK_DISPLAY_WVS7IN5V2B=1', language: ['c', 'cpp'])
endif

# Beautify logs, without these directives every log start with `../../`
add_global_arguments('-ffile-prefix-map=../../=', language: ['c', 'cpp'])
add_global_arguments('-fmacro-prefix-map=../../=', language: ['c', 'cpp'])

# ******************************************************************************
# *    Libraries
# ******************************************************************************
# cmake = import('cmake')

# poppler_opts = cmake.subproject_options()
# poppler_opts.add_cmake_defines({
#   'BUILD_GTK_TESTS': 'OFF',
#   'BUILD_QT5_TESTS': 'OFF',
#   'BUILD_QT6_TESTS': 'OFF',
#   'BUILD_CPP_TESTS': 'OFF',
#   'BUILD_MANUAL_TESTS': 'OFF',

#   'ENABLE_BOOST': 'OFF',
#   'ENABLE_UTILS': 'OFF',
#   'ENABLE_CPP': 'OFF',
#   'ENABLE_GLIB': 'OFF',
#   'ENABLE_GOBJECT_INTROSPECTION': 'OFF',
#   'ENABLE_GTK_DOC': 'OFF',
#   'ENABLE_QT5': 'OFF',
#   'ENABLE_QT6': 'OFF',
#   'ENABLE_LCMS': 'OFF',
#   'ENABLE_LIBCURL': 'OFF',
#   'ENABLE_LIBTIFF': 'OFF',
#   'ENABLE_NSS3': 'OFF',
#   'ENABLE_GPGME': 'OFF',

#   'BUILD_SHARED_LIBS': 'OFF',
#   'RUN_GPERF_IF_PRESENT': 'OFF',
#   'OpenJPEG_DIR': 'home/taba1uga/Github/stm32mp135f_ebook_reader/ebook_reader/subprojects/
  
#   # optional: avoid OpenJPEG entirely (JPX):
#   # 'ENABLE_LIBOPENJPEG': 'none',
# })

# poppler_sp = cmake.subproject('poppler', options: poppler_opts)
# dep_poppler = poppler_sp.dependency('poppler')
# ebook_reader_deps += [dep_poppler]

# Poppler is a bit messy when come to build, it require libopenjp2
#  to be installed before build which is not inline with meson build
#  model. So we declared these dependencies but do not add them to
#  ebook_reader_deps to force meson to download them.
dependency('libopenjp2',
 fallback: ['libopenjp2', 'libopenjp2_dep'],
 required: false,
)
dependency('poppler',
 fallback: ['poppler'],
 required: false,
)
poppler = custom_target('poppler',
  output: 'libpoppler.a',
  command: [
    'invoke',
    '-r', '../../ebook_reader/tools',
    '-c', 'poppler',
    'build',
  ],
  install: false,
)

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
		       ]),
		       # dep_poppler,
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
			 ), poppler]

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
