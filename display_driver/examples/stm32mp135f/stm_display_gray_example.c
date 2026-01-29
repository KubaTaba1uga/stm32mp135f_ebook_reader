#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "display_driver.h"
#include "tutrle_gray4.h"

/* static void usage(const char *prog) { */
/*   fprintf(stderr, */
/*           "Usage: %s [--wvs | --turtle | --cat_sm] [--help]\n" */
/*           "\n" */
/*           "Display a built-in image on the 7.5\" V2 e-paper panel (480x800).\n" */
/*           "\n" */
/*           "Options:\n" */
/*           "  --wvs       Show image_7in5_v2\n" */
/*           "  --turtle    Show turtle_7in5_v2\n" */
/*           "  --cat_sm    Show cat_sm_7in5_v2\n" */
/*           "  -h, --help  Show this help and exit\n", */
/*           prog); */
/* } */

int main(int argc, char *argv[]) {
  unsigned char *buf = (unsigned char *)turtle;
  int buf_len = sizeof(turtle);
  bool is_rotated = false;
  dd_error_t err;

  puts("Starting driver");
  err = init_stm32mp135f(dd_DisplayDriverEnum_Wvs7in5V2, is_rotated);
  if (err) {
    goto error;
  }

  puts("Working");
  err = dd_display_driver_clear(dd, true);
  if (err) {
    goto error_dd_cleanup;
  }

  
  puts("Working");
  err = dd_display_driver_write_gray(dd,buf, buf_len);
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
