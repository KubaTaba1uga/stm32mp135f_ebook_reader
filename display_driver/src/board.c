#include "board.h"
#include "gpio.h"

int board_init(struct gpio din, struct gpio clk, struct gpio cs, struct gpio dc,
               struct gpio rst, struct gpio busy, struct gpio pwr,
               struct board *out) {  
  *out = (struct board){
      .din = din,
      .clk = clk,
      .cs = cs,
      .dc = dc,
      .rst = rst,
      .busy = busy,
      .pwr = pwr,
  };

  return 0;
}
void board_destroy(struct board *out) {
  if (!out) return;

  gpio_destroy(&out->din);
  gpio_destroy(&out->clk);
  gpio_destroy(&out->cs);
  gpio_destroy(&out->dc);
  gpio_destroy(&out->rst);
  gpio_destroy(&out->busy);
  gpio_destroy(&out->pwr);
}
