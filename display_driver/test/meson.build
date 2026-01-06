unity_subproject = subproject('unity')
unity_dep = unity_subproject.get_variable('unity_dep')
unity_test_runner = unity_subproject.get_variable('gen_test_runner')

test_files = [
  'test_gpio.c',
  'test_list.c',
  'test_wvs75v2b.c',    
  # add other test_*.c files here
]

test_link_args = [
      '-Wl,--wrap=gpiod_chip_open',
      '-Wl,--wrap=gpiod_chip_close',
      '-Wl,--wrap=gpiod_chip_get_line',
      '-Wl,--wrap=gpiod_line_request_output',
      '-Wl,--wrap=gpiod_line_request_output_flags',      
      '-Wl,--wrap=gpiod_line_request_input',
      '-Wl,--wrap=gpiod_line_get_value',
      '-Wl,--wrap=gpiod_line_set_value',
      '-Wl,--wrap=gpiod_line_release',
      '-Wl,--wrap=dd_io_open',
      '-Wl,--wrap=dd_io_ioctl',
      '-Wl,--wrap=dd_io_close',
      # Add more mocks via: '-Wl,--wrap=<func name>'
]

test_deps = [unity_dep] + display_driver_deps

test_lib = static_library(
  'test_lib',
  sources: display_driver_src,
  dependencies: display_driver_deps,
  include_directories: display_driver_inc,
  c_args: ['-DENABLE_MOCKS'],
  link_args: test_link_args,
)

foreach test_file : test_files
  test_name = test_file.split('.')[0]    # e.g. "hep_parser_test"

  test_exe = executable(
    test_name,
    sources: [
      test_file,
      unity_test_runner.process(test_file),
      'conftest.c', 'conftest.h',
    ],
    dependencies: [
      unity_dep,
      display_driver_deps,
    ],
    include_directories: [
      include_directories('.'),
      display_driver_inc,
    ],
    c_args: ['-DENABLE_MOCKS'],
    link_with: test_lib,
    link_args: test_link_args,
  )

  test(test_name, test_exe)
endforeach

