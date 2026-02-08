#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "display_driver.h"

int main(void) {
  dd_error_t err;

  err = init_rpi4b();
  if (err) {
    goto error;
  }

  dd_display_driver_destroy(&dd);
  if (err) {
    goto error;
  }

  return EXIT_SUCCESS;

error: {
  char buf[1024];
  dd_error_dumps(err, 1024, buf);
  puts(buf);
}
  return EXIT_FAILURE;
}
