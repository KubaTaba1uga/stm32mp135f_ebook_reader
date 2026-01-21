#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "board.h"
#include "display_driver.h"
#include "penguin100x150.h"
#include "penguin256x256.h"

int main(int argc, char *argv[]) {
  dd_error_t err;

  puts("Starting driver");
  err = init_stm32mp135f(dd_DisplayDriverEnum_Wvs7in5V2, false);
  if (err) {
    goto error;
  }

  puts("Working");

  int x = 256;
  int y = 256;
  int bytes_per_row = (x + 7) / 8;
  int buf_len = bytes_per_row * y;
  
  err =  dd_display_driver_write_partial(dd, penguin2, buf_len, 0, x, 0, y);
  if (err) {
    goto error_dd_cleanup;
  }

   puts("I'm done");

  dd_display_driver_destroy(&dd);

  return EXIT_SUCCESS;

error_dd_cleanup:
  dd_display_driver_destroy(&dd);
error: {
  char buf[1024];
  dd_error_dumps(err, 1024, buf);
  puts(buf);
}
  return EXIT_FAILURE;
}
