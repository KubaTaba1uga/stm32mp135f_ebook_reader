#ifndef DISP_DRVER_BOARD_H
#define DISP_DRVER_BOARD_H
#include "gpio.h"

struct board {
  struct gpio din;
  struct gpio clk;
  struct gpio cs;
  struct gpio dc;
  struct gpio rst;
  struct gpio busy;
  struct gpio pwr;
};

int board_init(struct gpio din, struct gpio clk, struct gpio cs, struct gpio dc,
               struct gpio rst, struct gpio busy, struct gpio pwr,
               struct board *out);
void board_destroy(struct board *out);

#endif // DISP_DRVER_GPIO_H
