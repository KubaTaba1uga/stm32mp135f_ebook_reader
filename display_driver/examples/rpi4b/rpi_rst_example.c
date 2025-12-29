#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>

#include "display_driver.h"
#include "board.h"

int main(void) {
  dd_error_t err;

  puts("Starting driver");
  err = init_rpi4b();
  if (err) {
    goto error;
  }
  
  puts("Working");
  err = dd_wvs75v2b_ops_reset(dd);
  if (err) {
    goto error_dd_cleanup;
  }

  puts("I'm done");
  dd_wvs75v2b_destroy(&dd);

  return EXIT_SUCCESS;

error_dd_cleanup:
  dd_wvs75v2b_destroy(&dd);
error: {
  char buf[1024];
  dd_error_dumps(err, 1024, buf);
  puts(buf);
}
  return EXIT_FAILURE;
}
