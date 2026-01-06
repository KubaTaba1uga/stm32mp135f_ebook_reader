#ifndef CONFTEST_H
#define CONFTEST_H

#include <stdbool.h>

extern bool enable_gpiod_chip_open_mock;
extern int gpiod_chip_open_mock_called;

extern bool enable_gpiod_chip_close_mock;
extern int gpiod_chip_close_mock_called;

extern bool enable_gpiod_chip_get_line_mock;
extern int gpiod_chip_get_line_mock_called;

extern bool enable_gpiod_line_request_output_mock;
extern int gpiod_line_request_output_mock_called;

extern bool enable_gpiod_line_request_input_mock;
extern int gpiod_line_request_input_mock_called;

extern bool enable_gpiod_line_release_mock;
extern int gpiod_line_release_mock_called;

extern bool enable_gpiod_line_request_output_flags_mock;
extern int gpiod_line_request_output_flags_mock_called;

extern bool enable_gpiod_line_get_value_mock;
extern int gpiod_line_get_value_mock_called;
extern int gpiod_line_get_value_mock_return;

extern bool enable_gpiod_line_set_value_mock;
extern int gpiod_line_set_value_mock_called;

extern bool enable_open_mock;
extern int open_mock_called;
extern int open_mock_return;

extern bool enable_close_mock;
extern int close_mock_called;

extern bool enable_ioctl_mock;
extern int ioctl_mock_called;
extern int ioctl_mock_fail_after; 
extern int ioctl_mock_errno;

void gpiod_mock_reset_lines_pool(void);

#endif
