# Display driver
# ******************************************************************************
# *    Project
# ******************************************************************************
project('benchmarks', 'c',
         version: '0.0.1',
	 meson_version: '>=1.1',
	 default_options: [
            'werror=true',
            'c_std=gnu99',
         ],
)
add_global_arguments('-D_DEFAULT_SOURCE', language: ['c', 'cpp'])
add_global_arguments('-DEBK_DISPLAY_WVS7IN5V2', language: ['c', 'cpp'])

# ******************************************************************************
# *    Examples
# ******************************************************************************
rotate = executable('rotate',
  sources: ['rotate.c'],
)

render_pdf_page = executable('render_pdf_page',
  sources: ['render_pdf_page.c'],
)

lvgl_menu_demo_deps = [dependency('lvgl',
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
]

lvgl_menu_demo = executable('lvgl_menu_demo',
  sources: ['lvgl_menu_demo.c'],
  dependencies: lvgl_menu_demo_deps,  
)
