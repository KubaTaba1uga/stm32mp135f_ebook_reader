#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "display_driver.h"

static void usage(const char *prog) {
  fprintf(stderr,
          "Usage: %s [--white|--black]\n"
          "  --white   Clear to white (default)\n"
          "  --black   Clear to black\n",
          prog);
}

int main(int argc, char **argv) {
  bool is_clr_white = true;
  dd_error_t err;


  /* classic argv parsing */
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--white") == 0) {
      is_clr_white = true;
    } else if (strcmp(argv[i], "--black") == 0) {
      is_clr_white = false;
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
  err = init_rpi4b();
  if (err) {
    goto error;
  }

  puts("Working");
  err = dd_display_driver_clear(dd, is_clr_white);
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
