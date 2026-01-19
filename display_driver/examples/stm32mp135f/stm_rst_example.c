#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>

#include "display_driver.h"
#include "board.h"

int main(void) {
  dd_error_t err;

  puts("Starting driver");
  err = init_stm32mp135f(dd_DisplayDriverEnum_Wvs7in5V2);
  if (err) {
    goto error;
  }
  
  puts("I'm done");
  dd_display_driver_destroy(&dd);

  return EXIT_SUCCESS;

error: {
  char buf[1024];
  dd_error_dumps(err, 1024, buf);
  puts(buf);
}
  return EXIT_FAILURE;
}
