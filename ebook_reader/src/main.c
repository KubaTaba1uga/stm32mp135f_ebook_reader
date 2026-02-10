#include <fontconfig/fontconfig.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "app/app.h"
#include "utils/err.h"
#include "utils/log.h"

static app_t app;
static int main_setup(void);
static void exit_handler(void);
static void signal_handler(int);

int main(void) {
  log_info("Starting app");

  if (main_setup() != 0) {
    goto error_out;
  }

  err_o = app_init(&app);
  ERR_TRY(err_o);

  log_info("App is running");

  err_o = app_main(app);
  ERR_TRY(err_o);

  app_destroy(&app);

  return 0;

error_out:
  log_error(err_o);
  return err_o->code;
}

/**
   The e-ink displays that i used where prone to demage if left in power on
   state for too much time. So we are trying to fire up at least `panic` every
   time we detect that some unexpected crash happened.

   @todo Fork main to start app, detect exit code of app and do cleanup if it is
         not equal to 0.
*/
static void exit_handler(void) {
  if (app) {
    app_destroy(&app);
    FcFini(); // Font config leave some rubbish, we are cleaning it
              // cause it seems cleaner than adding asan suppresion
              // file.
  }
}

static void signal_handler(int signum) {
  if (app) {
    app_panic(app);
  }
  _exit(1);
};

static int main_setup(void) {
  struct sigaction sa = {0};
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  int signals[] = {
      SIGINT, SIGTERM, SIGHUP,
      SIGABRT, // SIGABRT is triggered by assert
  };
  for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); i++) {
    if (sigaction(signals[i], &sa, NULL) == -1) {
      err_o = err_errnof(errno, "Cannot sonfigure signal handler for %d",
                         signals[i]);
      return 1;
    }
  }
  if (atexit(exit_handler) != 0) {
    err_o = err_errnof(errno, "Cannot set exit handler");
    return 2;
  };

  return 0;
}

void __asan_on_error(void) {
  if (app) {
    app_panic(app);
  }
}
