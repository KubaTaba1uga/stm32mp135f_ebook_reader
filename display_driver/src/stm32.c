#include <stdio.h>
#include <stdlib.h>

#include "boards/board.h"
#include "boards/stm32mp135f_dk.h"
#include "utils/err.h"

int main(void) {
  dd_board_stm32mp135f_dk_t board;
  char err_buf[1024] = {0};

  if (dd_board_stm32mp135f_dk_init(&board) != 0) {
    goto error;
  };
  dd_board_t impl = dd_board_stm32mp135f_dk_get_board(board);

  puts("Powering the board on!");
  dd_board_power_on(impl);

  dd_board_stm32mp135f_dk_destroy(&board);

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
