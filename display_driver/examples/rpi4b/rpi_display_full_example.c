#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "display_driver.h"
#include "picture.h"


int main(void) {
  dd_image_t img;
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

  err = dd_wvs75v2b_ops_power_on(dd);
  if (err) {
    goto error_dd_cleanup;
  }

  /* err = */
  /*     dd_image_init(&img, (unsigned char *)image_7in5_v2, sizeof(image_7in5_v2), */
  /*                   (struct dd_ImagePoint){.x = 480, .y = 800}); */
  /* if (err) { */
  /*   goto error_dd_cleanup; */
  /* } */

  /* err = */
  /*     dd_image_init(&img, (unsigned char *)turtle_7in5_v2, sizeof(image_7in5_v2), */
  /*                   (struct dd_ImagePoint){.x = 480, .y = 800}); */
  /* if (err) { */
  /*   goto error_dd_cleanup; */
  /* } */
  err =
      dd_image_init(&img, (unsigned char *)super, sizeof(image_7in5_v2),
                    (struct dd_ImagePoint){.x = 480, .y = 800});
  if (err) {
    goto error_dd_cleanup;
  }

  
  err = dd_wvs75v2b_ops_display_full(dd, img);
  if (err) {
    goto error_img_cleanup;
  }

  err = dd_wvs75v2b_ops_power_off(dd);
  if (err) {
    goto error_img_cleanup;
  }

  puts("I'm done");

  dd_image_destroy(&img);
  dd_wvs75v2b_destroy(&dd);

  return EXIT_SUCCESS;

error_img_cleanup:
  dd_image_destroy(&img);
error_dd_cleanup:
  dd_wvs75v2b_destroy(&dd);
error: {
  char buf[1024];
  dd_error_dumps(err, 1024, buf);
  puts(buf);
}
  return EXIT_FAILURE;
}
