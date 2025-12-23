#ifndef DD_BOARD_H
#define DD_BOARD_H

#include "gpio.h"
#include "spi.h"

struct dd_board;
typedef struct dd_board *dd_board_t;
int dd_board_init(dd_gpio_pin_t cs, dd_gpio_pin_t dc, dd_gpio_pin_t rst, dd_gpio_pin_t bsy,
                  dd_gpio_pin_t pwr, dd_spi_t spi, dd_board_t *out);
void dd_board_destroy(dd_board_t *out);
int dd_board_power_on(dd_board_t board);

#endif // DD_GPIO_H
