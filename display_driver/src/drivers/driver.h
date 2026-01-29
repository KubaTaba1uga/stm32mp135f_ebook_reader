#ifndef DISPLAY_DRIVER_DRIVER_H
#define DISPLAY_DRIVER_DRIVER_H
#include "display_driver.h"

struct dd_DisplayDriver {
  dd_error_t (*write_part)(void *dd, unsigned char *buf, int buf_len, int x1,
                           int x2, int y1, int y2);
  dd_error_t (*write_fast)(void *dd, unsigned char *buf, int buf_len);
  dd_error_t (*write_gray)(void *dd, unsigned char *buf, int buf_len);  
  dd_error_t (*write)(void *dd, unsigned char *buf, int buf_len);
  dd_error_t (*clear)(void *dd, bool white);
  void (*destroy)(void *dd);

  void *driver_data;
  int stride;
  int x;
  int y;
};

void dd_driver_destroy(dd_display_driver_t *);
dd_error_t dd_driver_write(dd_display_driver_t, unsigned char *, int);
dd_error_t dd_driver_write_fast(dd_display_driver_t, unsigned char *, int);
dd_error_t dd_driver_write_part(dd_display_driver_t, unsigned char *, uint32_t,
                                int, int, int, int);
dd_error_t dd_driver_write_gray(dd_display_driver_t, unsigned char *, int);

dd_error_t dd_driver_clear(dd_display_driver_t, bool);
int dd_driver_get_x(dd_display_driver_t);
int dd_driver_get_y(dd_display_driver_t);
int dd_driver_get_stride(dd_display_driver_t);
dd_error_t dd_driver_wvs7in5v2_init(dd_display_driver_t, void *);
dd_error_t dd_driver_wvs7in5v2b_init(dd_display_driver_t, void *);

#endif // DISPLAY_DRIVER_DRIVER_H
