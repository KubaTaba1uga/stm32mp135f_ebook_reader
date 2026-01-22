#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "board.h"
#include "display_driver.h"
#include "penguin100x150.h"
#include "penguin160x120.h"
#include "penguin150x120.h"
#include "penguin150x100.h"
#include "penguin256x256.h"

int main(int argc, char *argv[]) {
  unsigned char *buf = (unsigned char *)penguin256x256;
  int buf_len = sizeof(penguin256x256);
  dd_error_t err;
  int x = 256;
  int y = 256;  


  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--100x150") == 0) {
      buf = (unsigned char *)penguin100x150;
      buf_len = sizeof(penguin100x150);
      x = 100;
      y=150;
    } else if (strcmp(argv[i], "--150x100") == 0) {
      buf = (unsigned char *)penguin150x100;
      buf_len = sizeof(penguin150x100);
      x = 150;
      y = 100;
    } else if (strcmp(argv[i], "--160x120") == 0) {
      buf = (unsigned char *)penguin160x120;
      buf_len = sizeof(penguin160x120);
      x = 160;
      y = 120;
    } else if (strcmp(argv[i], "--150x120") == 0) {
      buf = (unsigned char *)penguin150x120;
      buf_len = sizeof(penguin150x120);
      x = 150;
      y = 120;
    } 
    else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return EXIT_FAILURE;
    }
  }

  puts("Starting driver");
  err = init_stm32mp135f(dd_DisplayDriverEnum_Wvs7in5V2, false);
  if (err) {
    goto error;
  }

  puts("Working");

  int bytes_per_row = (x + 7) / 8;
  int new_buf_len = bytes_per_row * y;

  printf("Actual size=%d\n", buf_len);
  printf("Counted size=%d\n", new_buf_len);  
  
  err =  dd_display_driver_write_partial(dd, buf, buf_len, 0, x, 0, y);
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
