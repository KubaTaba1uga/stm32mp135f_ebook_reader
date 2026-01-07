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
ebook_reader_deps = []

ebook_reader_inc = include_directories(['src'])

ebook_reader_src = [files('src/main.c',
			  'src/utils/error.c',
			  'src/gui/gui.c',
			  'src/core/core.c',			 
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
