rotate = executable('bench_rotate',
  sources: ['rotate.c'],
)

render_pdf_page = executable('bench_render_pdf_page',
  sources: ['render_pdf_page.c'],
)

lvgl_menu_demo_deps = ebook_reader_deps

lvgl_menu_demo = executable('bench_lvgl_menu_demo',
  sources: ['lvgl_menu_demo.c'],
  dependencies: lvgl_menu_demo_deps + [ebook_reader_dep],
  include_directories: ebook_reader_inc,
)
