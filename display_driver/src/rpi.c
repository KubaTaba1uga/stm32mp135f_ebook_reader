#include <stdio.h>
#include <stdlib.h>

#include "boards/board.h"
#include "boards/raspberrypi_5.h"
#include "utils/err.h"

int main(void) {
  dd_board_raspberrypi_5_t board;
  char err_buf[1024] = {0};

  if (dd_board_raspberrypi_5_init(&board) != 0) {
    goto error;
  };
  dd_board_t impl = dd_board_raspberrypi_5_get_board(board);

  puts("Powering the board on!");
  if (dd_board_power_on(impl) != 0) {
    goto error;
    };

  dd_board_raspberrypi_5_destroy(&board);

  return EXIT_SUCCESS;

error:
  if (dd_errno) {
    dd_error_dumps(dd_errno, sizeof(err_buf), err_buf);
    printf("%s\n", err_buf);
  } else {
    puts("Unkown error");
  }

  return EXIT_FAILURE;
}
