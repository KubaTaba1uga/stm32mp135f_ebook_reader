#ifndef DISPLAY_DRIVER_DRIVER_H
#define DISPLAY_DRIVER_DRIVER_H
#include <stdint.h>

#include "display_driver.h"

dd_error_t
dd_driver_create(dd_display_driver_t *out, int x, int y, int stride,
                 void (*remove)(void *), dd_error_t (*clear)(void *, bool),
                 dd_error_t (*write)(void *, unsigned char *, uint32_t),
                 void *driver_data);
void dd_driver_destroy(dd_display_driver_t *out);
dd_error_t dd_driver_write(dd_display_driver_t driver, unsigned char *buf,
                           int buf_len);
dd_error_t dd_driver_clear(dd_display_driver_t driver, bool white);
int dd_driver_get_x(dd_display_driver_t driver);
int dd_driver_get_y(dd_display_driver_t driver);
int dd_driver_get_stride(dd_display_driver_t driver);

#endif // DISPLAY_DRIVER_DRIVER_H
