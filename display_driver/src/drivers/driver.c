#include <stdlib.h>
#include <string.h>

#include "display_driver.h"
#include "drivers/driver.h"
#include "drivers/waveshare_7in5_V2b.h"
#include "utils/err.h"

dd_error_t dd_init(dd_diplay_driver_t driver, enum dd_DisplayDriverEnum model,
                          void *config) {
  switch (model) {
  case dd_DisplayDriverEnum_Wvs7in5V2b:
    driver->probe = dd_wvs75v2b_probe;
    break;
  default:
    dd_errno = dd_errnos(EINVAL, "Unsupported display model");
    goto error;
  }

  if (!driver->probe(driver, config)) {
    dd_errno = dd_errnos(ENODATA, "No matching driver for display model");
    goto error;
  };

  return 0;

error:
  *driver = (struct dd_DisplayDriver){0};
  return dd_errno;
}

void dd_destroy(dd_diplay_driver_t driver) {
  if (driver->remove) {
    driver->remove(driver);
  }

  *driver = (struct dd_DisplayDriver){0};
};

dd_error_t dd_write(dd_diplay_driver_t driver, unsigned char *buf,
                           uint32_t buf_len) {
  if (!driver->write) {
    dd_errno =
        dd_errnos(EINVAL, "Write operation is not supported on this display");
    goto error;
  }

  dd_errno = driver->write(driver, buf, buf_len);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}

dd_error_t dd_clear(dd_diplay_driver_t driver, bool white) {
  if (!driver->clear) {
    dd_errno =
        dd_errnos(EINVAL, "Clear operation is not supported on this display");
    goto error;
  }

  dd_errno = driver->clear(driver, white);
  DD_TRY(dd_errno);

  return 0;

error:
  return dd_errno;
}
