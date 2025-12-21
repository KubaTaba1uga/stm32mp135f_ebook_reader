#ifndef DD_BOARD_H
#define DD_BOARD_H

#include "gpio.h"

struct dd_board;
typedef struct dd_board *dd_board_t;
int dd_board_init(dd_gpio_pin_t din, dd_gpio_pin_t clk, dd_gpio_pin_t cs,
                  dd_gpio_pin_t dc, dd_gpio_pin_t rst, dd_gpio_pin_t bsy,
                  dd_gpio_pin_t pwr, dd_board_t *out);
void dd_board_destroy(dd_board_t *out);

#endif // DD_GPIO_H
