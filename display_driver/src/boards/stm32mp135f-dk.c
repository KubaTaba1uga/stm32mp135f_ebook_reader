#include "stm32mp135f-dk.h"
#include "board.h"
#include "gpio.h"
#include "utils/_err.h"
#include "utils/mem.h"

struct dd_board_stm32mp135f_dk {
  dd_gpio_bank_t gpioa;
  dd_gpio_bank_t gpioc;
  dd_gpio_bank_t gpiog;
  dd_gpio_bank_t gpioh;
  dd_gpio_bank_t gpioi;
  dd_board_t board;
};

typedef struct dd_board_stm32mp135f_dk *dd_board_stm32mp135f_dk_t;
int dd_board_stm32mp135f_dk_init(dd_board_stm32mp135f_dk_t *out) {
  dd_gpio_bank_t gpioa;
  dd_gpio_bank_t gpioc;
  dd_gpio_bank_t gpiog;
  dd_gpio_bank_t gpioh;
  dd_gpio_bank_t gpioi;

  if (dd_gpio_bank_init('A', &gpioa) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_bank_init('C', &gpioc) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_bank_init('G', &gpiog) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_bank_init('H', &gpioh) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_bank_init('I', &gpioi) != 0) {
    dd_ewrap();
    goto error;
  }

  dd_gpio_pin_t din;
  dd_gpio_pin_t clk;
  dd_gpio_pin_t cs;
  dd_gpio_pin_t dc;
  dd_gpio_pin_t rst;
  dd_gpio_pin_t bsy;
  dd_gpio_pin_t pwr;

  if (dd_gpio_pin_init(3, dd_gpio_dir_output, gpioh, &din) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(7, dd_gpio_dir_output, gpioh, &clk) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(11, dd_gpio_dir_output, gpioh, &cs) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(0, dd_gpio_dir_output, gpioi, &dc) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(2, dd_gpio_dir_output, gpioc, &rst) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(3, dd_gpio_dir_input, gpiog, &bsy) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(4, dd_gpio_dir_output, gpioa, &pwr) != 0) {
    dd_ewrap();
    goto error;
  }

  dd_board_t board;
  if (dd_board_init(din, clk, cs, dc, rst, bsy, pwr, &board) != 0) {
    dd_ewrap();
    goto error;
  };

  *out = dd_malloc(sizeof(struct dd_board_stm32mp135f_dk));
  **out = (struct dd_board_stm32mp135f_dk){
      .board = board,
      .gpioa = gpioa,
      .gpiog = gpiog,
      .gpioc = gpioc,
      .gpioh = gpioh,
      .gpioi = gpioi,
  };

  return 0;

error:
  return dd_ereturn(-1);
}

void dd_board_stm32mp135f_dk_destroy(dd_board_stm32mp135f_dk_t *out) {
  if (!out || !*out) {
    return;
  }

  dd_board_destroy(&(*out)->board);
  dd_gpio_bank_destroy(&(*out)->gpioa);
  dd_gpio_bank_destroy(&(*out)->gpioc);
  dd_gpio_bank_destroy(&(*out)->gpiog);
  dd_gpio_bank_destroy(&(*out)->gpioh);
  dd_gpio_bank_destroy(&(*out)->gpioi);

  *out = NULL;
};
