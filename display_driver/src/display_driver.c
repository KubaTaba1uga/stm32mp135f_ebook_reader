#include "display_driver.h"
#include "drivers/driver.h"
#include "utils/err.h"
#include "utils/mem.h"

dd_error_t dd_display_driver_init(dd_display_driver_t *out,
                                  enum dd_DisplayDriverEnum model,
                                  void *config) {
  if (!out || !config) {
    dd_errno = dd_errnos(EINVAL, "`out` and `config` cannot be NULL");
    goto error_out;
  }

  *out = dd_malloc(sizeof(struct dd_DisplayDriver));

  switch (model) {
  case dd_DisplayDriverEnum_Wvs7in5V2:
    dd_errno = dd_driver_wvs7in5v2_init(*out, config);
    DD_TRY_CATCH(dd_errno, error_out_cleanup);
    break;
  case dd_DisplayDriverEnum_Wvs7in5V2b:
    dd_errno = dd_driver_wvs7in5v2b_init(*out, config);
    DD_TRY_CATCH(dd_errno, error_out_cleanup);
    break;
  }

  return 0;

error_out_cleanup:
  dd_free(*out);
  *out = NULL;
error_out:
  return dd_errno;
}

void dd_display_driver_destroy(dd_display_driver_t *out) {
  if (!out || !*out) {
    return;
  }

  dd_driver_destroy(out);
  dd_free(*out);
  *out = NULL;
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

int dd_display_driver_get_x(dd_display_driver_t dd) {
  if (!dd) {
    dd_errno = dd_errnos(EINVAL, "`dd` cannot be NULL");
    goto error_out;
  }

  return dd_driver_get_x(dd);

error_out:
  return -1;
}

int dd_display_driver_get_y(dd_display_driver_t dd) {
  if (!dd) {
    dd_errno = dd_errnos(EINVAL, "`dd` cannot be NULL");
    goto error_out;
  }

  return dd_driver_get_y(dd);

error_out:
  return -1;
}

int dd_display_driver_get_stride(dd_display_driver_t dd) {
  if (!dd) {
    dd_errno = dd_errnos(EINVAL, "`dd` cannot be NULL");
    goto error_out;
  }

  return dd_driver_get_stride(dd);

error_out:
  return -1;
}

dd_error_t dd_display_driver_write_partial(dd_display_driver_t dd,
                                           unsigned char *buf, uint32_t buf_len,
                                           int x1, int x2, int y1, int y2) {
  if (!dd || !buf) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `buf` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_driver_write_part(dd, buf, buf_len, x1, x2, y1, y2);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

dd_error_t dd_display_driver_write_fast(dd_display_driver_t dd,
                                        unsigned char *buf, uint32_t buf_len) {
  if (!dd || !buf) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `buf` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_driver_write_fast(dd, buf, buf_len);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}
