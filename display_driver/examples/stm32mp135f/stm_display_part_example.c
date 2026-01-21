#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "board.h"
#include "display_driver.h"

int main(int argc, char *argv[]) {
  dd_error_t err;

  puts("Starting driver");
  err = init_stm32mp135f(dd_DisplayDriverEnum_Wvs7in5V2, false);
  if (err) {
    goto error;
  }

  puts("Working");  
  int start_x = 0;
  int start_y = 0;  
  int square_x = 64;
  int square_y = 64;
  unsigned char square_black[64 * 64 / 8];
  unsigned char square_white[64 * 64 / 8];
  memset(square_black, 0xFF, sizeof(square_black));
  memset(square_white, 0x00, sizeof(square_white));

  int bytes_per_row = (square_x + 7) / 8;
  int buf_len = bytes_per_row * square_y;

  for(int i=0; i<3; i++){
    err = dd_display_driver_write_partial(dd, square_black, buf_len, start_x,
                                          start_x + square_x, start_y, start_y + square_y);
    if (err) {
      goto error_dd_cleanup;
    }

    start_x += square_x;
    start_y += square_y;
  }

  for (int i = 3; i > 0; i--) {
    err = dd_display_driver_write_partial(dd, square_white, buf_len, start_x - square_x * i,
                                          start_x + square_x - square_x * i, start_y - square_y * i, start_y + square_y - square_y * i);
    if (err) {
      goto error_dd_cleanup;
    }
  }
  
  int rect_x = 150;
  int rect_y = 100;
  unsigned char rect_black[100 * 150];
  unsigned char rect_white[100 * 150];
  memset(rect_black, 0xFF, sizeof(rect_black));
  memset(rect_white, 0x00, sizeof(rect_white));

   bytes_per_row = (rect_x + 7) / 8;
   buf_len = bytes_per_row * rect_y;

  for(int i=0; i<3; i++){
    err = dd_display_driver_write_partial(dd, rect_black, buf_len, start_x,
                                          start_x + rect_x, start_y, start_y + rect_y);
    if (err) {
      goto error_dd_cleanup;
    }

    start_x += rect_x;
    start_y += rect_y;
  }

  for (int i = 3; i > 0; i--) {
    err = dd_display_driver_write_partial(dd, rect_white, buf_len, start_x - rect_x * i,
                                          start_x + rect_x - rect_x * i, start_y - rect_y * i, start_y + rect_y - rect_y * i);
    if (err) {
      goto error_dd_cleanup;
    }
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
