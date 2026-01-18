unity_subproject = subproject('unity')
unity_dep = unity_subproject.get_variable('unity_dep')
unity_test_runner = unity_subproject.get_variable('gen_test_runner')

test_files = [
  'test_unity.c',
  'test_list.c',  
  # add other test_*.c files here
]

# test_link_args = [
#       '-Wl,--wrap=ebk_display_init',
#       '-Wl,--wrap=ebk_display_show_boot_img',
#       '-Wl,--wrap=ebk_display_destroy',
#       '-Wl,--wrap=ebk_gui_init',
#       '-Wl,--wrap=ebk_gui_tick',
#       '-Wl,--wrap=ebk_gui_destroy',
#       # Add more mocks via: '-Wl,--wrap=<func name>'
# ]

test_deps = [unity_dep] + ebook_reader_deps

test_lib = static_library(
  'test_lib',
  sources: ebook_reader_src,
  dependencies: ebook_reader_deps,
  include_directories: ebook_reader_inc,
  # link_args: test_link_args,
)

foreach test_file : test_files
  test_name = test_file.split('.')[0]    # e.g. "hep_parser_test"

  test_exe = executable(
    test_name,
    sources: [
      test_file,
      unity_test_runner.process(test_file),
      # 'conftest.c', 'conftest.h',
    ],
    dependencies: [
      unity_dep,
      ebook_reader_deps,
    ],
    include_directories: [
      include_directories('.'),
      ebook_reader_inc,
    ],
    link_with: test_lib,
    # link_args: test_link_args,
  )

  test(test_name, test_exe)
endforeach