#ifndef DISPLAY_DRIVER_DRIVER_H
#define DISPLAY_DRIVER_DRIVER_H
#include <stdint.h>

#include "display_driver.h"

struct dd_DisplayDriver {
  // Detect whether display can be configured, create new driver instance and
  // assign supported ops.
  dd_error_t (*probe)(struct dd_DisplayDriver *dd, void *config);

  // Delete driver instance.
  void (*remove)(struct dd_DisplayDriver *dd);

  // Clear screen.
  dd_error_t (*clear)(struct dd_DisplayDriver *dd, bool white);

  // Write buf to screen with full refresh.
  dd_error_t (*write)(struct dd_DisplayDriver *dd, unsigned char *buf,
                      uint32_t buf_len);

  // Placeholder for driver instance data.
  void *driver_data;
};

dd_error_t dd_init(dd_diplay_driver_t driver, enum dd_DisplayDriverEnum model,
                   void *config);
void dd_destroy(dd_diplay_driver_t driver);
dd_error_t dd_write(dd_diplay_driver_t driver, unsigned char *buf,
                    uint32_t buf_len);
dd_error_t dd_clear(dd_diplay_driver_t driver, bool white);

#endif // DISPLAY_DRIVER_DRIVER_H
