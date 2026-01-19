#include "display_driver.h"
#include "drivers/waveshare_7in5_V2b.h"
#include "drivers/waveshare_7in5_V2.h"
#include "utils/err.h"
#include "utils/mem.h"

dd_error_t dd_display_driver_init(dd_display_driver_t *out,
                                  enum dd_DisplayDriverEnum model,
                                  void *config) {
  if (!out || !config) {
    dd_errno = dd_errnos(EINVAL, "`out` and `config` cannot be NULL");
    goto error_out;
  }

  switch (model) {
  case dd_DisplayDriverEnum_Wvs7in5V2:
    dd_errno = dd_driver_wvs7in5v2_create(out, config);
    DD_TRY_CATCH(dd_errno, error_out_cleanup);
    break;
  case dd_DisplayDriverEnum_Wvs7in5V2b:
    dd_errno = dd_driver_wvs7in5v2b_create(out, config);
    DD_TRY_CATCH(dd_errno, error_out_cleanup);
    break;
    
  }

  return 0;

error_out_cleanup:
  dd_free((void *)*out);
  *out = NULL;
error_out:
  return dd_errno;
}

void dd_display_driver_destroy(dd_display_driver_t *out) {
  if (!out || !*out) {
    return;
  }

  dd_driver_destroy(out);
}

dd_error_t dd_display_driver_clear(dd_display_driver_t dd, bool white) {
  if (!dd) {
    dd_errno = dd_errnos(EINVAL, "`out` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_driver_clear(dd, white);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

/**
   @brief Display picture on whole screen.
   @param dd Driver instance.
   @param image Image to displayed.
   @return Error on failure, NULL on success.
 */
dd_error_t dd_display_driver_write(dd_display_driver_t dd, unsigned char *buf,
                                   uint32_t buf_len) {
  if (!dd || !buf) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `buf` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_driver_write(dd, buf, buf_len);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}
