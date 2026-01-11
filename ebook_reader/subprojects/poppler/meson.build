# ******************************************************************************
# *    Project
# ******************************************************************************
project('poppler', 'c',
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
dependency('libopenjp2',
 fallback: ['libopenjp2', 'libopenjp2_dep'],
 required: false,
)
dependency('cmake_poppler',
 fallback: ['cmake_poppler'],
 required: false,
)
poppler_src = custom_target('poppler',
  output: 'libpoppler.a',
  command: [
    'invoke',
    '-r', meson.project_source_root() / 'tools',
    '-c', 'poppler',
    'build',
  ],
  install: false,
)
poppler_lib = library('poppler',
     sources: [poppler_src],
     include_directories: include_directories(['include'])
)
poppler_dep = declare_dependency(
  link_with: poppler_lib,
)
