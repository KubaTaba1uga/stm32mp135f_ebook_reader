#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "display_driver.h"
#include "tutrle_gray4.h"
#include "cat_gray4.h"
#include "cat_gray4_roatated.h"

static void usage(const char *prog) {
  fprintf(stderr,
          "Usage: %s [--wvs | --turtle | --cat_sm] [--help]\n"
          "\n"
          "Display an image on the 7.5\" V2 e-paper panel (480x800) in 4 level grayscale.\n"
          "\n"
          "Options:\n"
          "  --turtle    Show turtle\n"
          "  --cat    Show cat\n"
          "  -h, --help  Show this help and exit\n",
          prog);
}

int main(int argc, char *argv[]) {
  unsigned char *buf = (unsigned char *)turtle;
  int buf_len = sizeof(turtle);
  bool is_rotated = false;
  dd_error_t err;


  /* classic argv parsing */
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--cat_rotated") == 0) {
      buf = (unsigned char *)cat_rotated;
      buf_len = sizeof(cat_rotated);
    }else    if (strcmp(argv[i], "--cat") == 0) {
      buf = (unsigned char *)cat;
      buf_len = sizeof(cat);
      is_rotated = true;
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  }
  
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
