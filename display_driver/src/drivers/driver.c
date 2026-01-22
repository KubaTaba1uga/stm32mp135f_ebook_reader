#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "utils/err.h"
#include "utils/mem.h"

void dd_driver_destroy(dd_display_driver_t *out) {
  if (!out || !*out) {
    return;
  }

  if ((*out)->destroy) {
    (*out)->destroy(((*out)->driver_data));
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

dd_error_t dd_driver_write_part(dd_display_driver_t driver, unsigned char *buf,
                                uint32_t buf_len, int x1, int x2, int y1,
                                int y2) {

  if (!driver->write_part) {
    dd_errno = dd_errnos(
        EINVAL, "Partial write operation is not supported on this display");
    goto error_out;
  }

  dd_errno =
      driver->write_part(driver->driver_data, buf, buf_len, x1, x2, y1, y2);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

dd_error_t dd_driver_write_fast(dd_display_driver_t driver, unsigned char *buf,
                                int buf_len) {

  if (!driver->write_fast) {
    dd_errno = dd_errnos(
        EINVAL, "Fast write operation is not supported on this display");
    goto error_out;
  }

  dd_errno = driver->write_fast(driver->driver_data, buf, buf_len);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}
