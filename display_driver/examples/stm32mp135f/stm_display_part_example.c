#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../rpi4b/picture.h"
#include "board.h"
#include "display_driver.h"

/* static void usage(const char *prog) { */
/*   fprintf(stderr, */
/*           "Usage: %s [--wvs | --turtle | --cat_sm] [--help]\n" */
/*           "\n" */
/*           "Display a built-in image on the 7.5\" V2 e-paper panel
 * (480x800).\n" */
/*           "\n" */
/*           "Options:\n" */
/*           "  --wvs       Show image_7in5_v2\n" */
/*           "  --turtle    Show turtle_7in5_v2\n" */
/*           "  --cat_sm    Show cat_sm_7in5_v2\n" */
/*           "  -h, --help  Show this help and exit\n", */
/*           prog); */
/* } */

int main(int argc, char *argv[]) {
  /* unsigned char *buf = (unsigned char *)turtle_7in5_v2; */
  /* int buf_len = sizeof(turtle_7in5_v2); */
  /* bool is_rotated = false; */
  dd_error_t err;

  puts("Starting driver");
  err = init_stm32mp135f(dd_DisplayDriverEnum_Wvs7in5V2, false);
  if (err) {
    goto error;
  }

  const int x = 20;
  const int y = 20;
  unsigned char black_buf[20 * 20];
  unsigned char white_buf[20 * 20];
  int start_x = 0;
  int start_y = 240;
  puts("Working");
  memset(black_buf, 0xFF, sizeof(black_buf));
  memset(white_buf, 0x00, sizeof(white_buf));

  
  int bytes_per_row = (x + 7) / 8;
  int buf_len = bytes_per_row * y;

  puts("PARTIAL START");

  /* int x_mod = 1; */
  /* int y_mod = 1; */
  int i = 0;
  while (true) {
    err = dd_display_driver_write_partial(dd, black_buf, buf_len, start_x,
                                          start_x + x, start_y, start_y + y);
    if (err) {
      goto error_dd_cleanup;
    }
    sleep(2);
    if (i >= 3) {
      err = dd_display_driver_write_partial(dd, white_buf, buf_len, start_x - x*3,
                                            start_x -x*2, start_y, start_y + y);
      if (err) {
        goto error_dd_cleanup;
      }
    }
      puts("");
    /* if (start_x + x >= 800) { */
    /*   x_mod = -1; */
    /* } */
    /* if (start_y + y >= 480) { */

    /*   y_mod = -1; */
    /* } */
    i++;
    start_x += x;
    /* start_y += y * y_mod; */
  }
  /* err = dd_display_driver_write_partial(dd, buf, buf_len, start_x, */
  /*                                       start_x + x, start_y, start_y + y);
   */
  /* if (err) { */
  /*   goto error_dd_cleanup; */
  /* } */

  /* start_x += x; */
  /* start_y += y; */
  /* err = dd_display_driver_write_partial(dd, buf, buf_len, start_x, */
  /*                                       start_x + x, start_y, start_y + y);
   */
  /* if (err) { */
  /*   goto error_dd_cleanup; */
  /* } */

  /* start_x += x; */
  /* start_y += y;   */
  /* x = 50; */
  /* y = 100; */
  /* err = dd_display_driver_write_partial(dd, buf, buf_len, start_x, */
  /*                                       start_x + x, start_y, start_y + y);
   */
  /* if (err) { */
  /*   goto error_dd_cleanup; */
  /* } */

  /* start_x += x; */
  /* start_y += y; */
  /* err = dd_display_driver_write_partial(dd, buf, buf_len, start_x, */
  /*                                       start_x + x, start_y, start_y + y);
   */
  /* if (err) { */
  /*   goto error_dd_cleanup; */
  /* } */

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
