#ifndef CONFTEST_H
#define CONFTEST_H

#include <stdbool.h>

extern bool enable_gpiod_chip_open_mock;
extern int gpiod_chip_open_mock_called;

extern bool enable_gpiod_chip_close_mock;
extern int gpiod_chip_close_mock_called;

#endif
