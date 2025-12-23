#include "boards/raspberrypi_5.h"
#include "boards/board.h"
#include "gpio.h"
#include "spi.h"
#include "utils/err.h"
#include "utils/mem.h"

struct dd_board_raspberrypi_5 {
  dd_gpio_bank_t gpioa;
  dd_board_t board;
};

typedef struct dd_board_raspberrypi_5 *dd_board_raspberrypi_5_t;
int dd_board_raspberrypi_5_init(dd_board_raspberrypi_5_t *out) {
  dd_gpio_bank_t gpioa;

  if (dd_gpio_bank_init('A', &gpioa) != 0) {
    dd_ewrap();
    goto error;
  }

  dd_gpio_pin_t cs = {0};
  dd_gpio_pin_t dc;
  dd_gpio_pin_t rst;
  dd_gpio_pin_t bsy;
  dd_gpio_pin_t pwr;

  (void)cs;
  /* if (dd_gpio_pin_init(8, dd_gpio_dir_output, gpioa, &cs) != 0) { */
  /*   dd_ewrap(); */
  /*   goto error; */
  /* } */
  
  if (dd_gpio_pin_init(25, dd_gpio_dir_output, gpioa, &dc) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(17, dd_gpio_dir_output, gpioa, &rst) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(24, dd_gpio_dir_input, gpioa, &bsy) != 0) {
    dd_ewrap();
    goto error;
  }

  if (dd_gpio_pin_init(18, dd_gpio_dir_output, gpioa, &pwr) != 0) {
    dd_ewrap();
    goto error;
  }
  
  dd_spi_t spi;
  if (dd_spi_init("/dev/spidev0.0", &spi) == -1) {
    dd_ewrap();
    goto error;
  }

  dd_board_t board;
  if (dd_board_init(cs, dc, rst, bsy, pwr, spi, &board) != 0) {
    dd_ewrap();
    goto error;
  };

  *out = dd_malloc(sizeof(struct dd_board_raspberrypi_5));
  **out = (struct dd_board_raspberrypi_5){
      .board = board,
      .gpioa = gpioa,
  };

  return 0;

error:
  return dd_ereturn(-1);
}

void dd_board_raspberrypi_5_destroy(dd_board_raspberrypi_5_t *out) {
  if (!out || !*out) {
    return;
  }

  dd_board_destroy(&(*out)->board);
  dd_gpio_bank_destroy(&(*out)->gpioa);
  *out = NULL;
};

dd_board_t dd_board_raspberrypi_5_get_board(dd_board_raspberrypi_5_t board) {
  return board->board;
}
