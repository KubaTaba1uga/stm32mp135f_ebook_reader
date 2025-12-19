#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "stm32board.h"

int main(void) {
  struct stm32board board;
  cdk_errno = 0;

  int err = stm32board_init(&board);
  if (err) {
    goto error;
  }

  return EXIT_SUCCESS;

error: {
  if (cdk_errno) {
    char err_buf[255] = {0};
    cdk_error_dumps(cdk_errno, sizeof(err_buf), err_buf);
    printf("%s\n", err_buf);
  } else {
    puts("Unkown error");
  }

  return EXIT_FAILURE;
}
}
