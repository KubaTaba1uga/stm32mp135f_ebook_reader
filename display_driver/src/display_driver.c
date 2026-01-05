#include "display_driver.h"
#include "drivers/driver.h"
#include "utils/err.h"
#include "utils/mem.h"

dd_error_t dd_diplay_driver_init(dd_diplay_driver_t *out,
                                 enum dd_DisplayDriverEnum model,
                                 void *config) {
  if (!out || !config) {
    dd_errno = dd_errnos(EINVAL, "`out` and `config` cannot be NULL");
    goto error_out;
  }

  *out = dd_malloc(sizeof(struct dd_DisplayDriver));

  dd_errno = dd_init(*out, model, config);
  DD_TRY_CATCH(dd_errno, error_out_cleanup);

  return 0;

error_out_cleanup:
  dd_free((void *)*out);
error_out:
  return dd_errno;
}

void dd_diplay_driver_destroy(dd_diplay_driver_t *out) {
  if (!out || !*out) {
    return;
  }

  dd_destroy(*out);
  *out = NULL;
}

dd_error_t dd_diplay_driver_clear(dd_diplay_driver_t dd, bool white) {
  if (!dd) {
    dd_errno = dd_errnos(dd, "`out` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_clear(dd, white);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}

/**
 * \brief Display picture on whole screen.
 * \param dd Driver instance.
 * \param image Image to displayed.
 * \return Error on failure, NULL on success.
 */
dd_error_t dd_diplay_driver_write(dd_diplay_driver_t dd, unsigned char *buf,
                                  uint32_t buf_len) {
  if (!dd || !buf) {
    dd_errno = dd_errnos(EINVAL, "`dd` and `buf` cannot be NULL");
    goto error_out;
  }

  dd_errno = dd_write(dd, buf, buf_len);
  DD_TRY(dd_errno);

  return 0;

error_out_cleanup:
  dd_free((void *)*out);
error_out:
  return dd_errno;
}
