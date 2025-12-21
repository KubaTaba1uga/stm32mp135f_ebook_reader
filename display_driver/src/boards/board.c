#include "board.h"
#include "utils/mem.h"

struct dd_board {
  dd_gpio_pin_t din;
  dd_gpio_pin_t clk;
  dd_gpio_pin_t cs;
  dd_gpio_pin_t dc;
  dd_gpio_pin_t rst;
  dd_gpio_pin_t bsy;
  dd_gpio_pin_t pwr;
};

int dd_board_init(dd_gpio_pin_t din, dd_gpio_pin_t clk, dd_gpio_pin_t cs,
                  dd_gpio_pin_t dc, dd_gpio_pin_t rst, dd_gpio_pin_t bsy,
                  dd_gpio_pin_t pwr, dd_board_t *out) {
  *out = dd_malloc(sizeof(struct dd_board));

  **out = (struct dd_board){
      .din = din,
      .clk = clk,
      .cs = cs,
      .dc = dc,
      .rst = rst,
      .bsy = bsy,
      .pwr = pwr,
  };

  return 0;
}

void dd_board_destroy(dd_board_t *out) {
  if (!out || !*out) {
    return;
  }

  dd_free(*out);
  *out = NULL;
};
