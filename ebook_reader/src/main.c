#include <stdbool.h>

#include "app/app.h"
#include "utils/err.h"
#include "utils/log.h"

#define LOG_ERROR(err)                                                         \
  if (!is_err_logged) {                                                        \
    log_error(err);                                                            \
    is_err_logged = true;                                                      \
  }

static app_t app;

int main(void) {
  bool is_err_logged = false; // We need this var cause cleanup routines can
                              // set different errno along the cleanup way and
                              // we need to ensure that we log only error in the
                              // highest error_X_cleanup routine.
  err_errno = app_init(&app);

  ERR_TRY(err_errno);

  log_info("App is running");

  err_errno = app_main(app);
  ERR_TRY_CATCH(err_errno, error_core_cleanup);

  app_destroy(&app);

  return 0;

error_core_cleanup:
  LOG_ERROR(err_errno);
  app_destroy(&app);
error_out:
  LOG_ERROR(err_errno);
  return err_errno->code;

  return 0;
}
