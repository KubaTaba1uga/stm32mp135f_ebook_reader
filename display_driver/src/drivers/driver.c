#include <stdlib.h>
#include <string.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "utils/err.h"
#include "utils/mem.h"

struct dd_DisplayDriver {
  dd_error_t (*write)(void *dd, unsigned char *buf, uint32_t buf_len);
  dd_error_t (*clear)(void *dd, bool white);
  void (*remove)(void *dd);

  void *driver_data;
  int stride;
  int x;
  int y;
};

dd_error_t
dd_driver_create(dd_display_driver_t *out, int x, int y, int stride,
                 void (*remove)(void *), dd_error_t (*clear)(void *, bool),
                 dd_error_t (*write)(void *, unsigned char *, uint32_t),
                 void *driver_data) {

  dd_display_driver_t dd = *out = dd_malloc(sizeof(struct dd_DisplayDriver));
  *dd = (struct dd_DisplayDriver){
      .x = x,
      .y = y,
      .stride = stride,
      .remove = remove,
      .write = write,
      .clear = clear,
      .driver_data = driver_data,
  };

  return 0;
}

void dd_driver_destroy(dd_display_driver_t *out) {
  if (!out || !*out) {
    return;
  }

  if ((*out)->remove) {
    (*out)->remove(((*out)->driver_data));
  }

  dd_free(*out);
  *out = NULL;
};

dd_error_t dd_driver_write(dd_display_driver_t driver, unsigned char *buf,
                           int buf_len) {
  if (!driver->write) {
    dd_errno =
        dd_errnos(EINVAL, "Write operation is not supported on this display");
    goto error_out;
  }

  dd_errno = driver->write(driver->driver_data, buf, buf_len);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

dd_error_t dd_driver_clear(dd_display_driver_t driver, bool white) {
  if (!driver->clear) {
    dd_errno =
        dd_errnos(EINVAL, "Clear operation is not supported on this display");
    goto error_out;
  }

  dd_errno = driver->clear(driver->driver_data, white);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

int dd_driver_get_x(dd_display_driver_t driver) { return driver->x; }
int dd_driver_get_y(dd_display_driver_t driver) { return driver->y; }
int dd_driver_get_stride(dd_display_driver_t driver) { return driver->stride; }
