#include <lvgl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gui/gui.h"
#include "utils/error.h"
#include "utils/log.h"

struct EbookReader {
  gui_t gui;
};

static struct EbookReader ereader;

static int configure_main(void);
static void signal_handler(int);
static void exit_handler(void);

int main(void) {
  puts(__func__);
  if (!configure_main()) {
    return EXIT_FAILURE;
  }

  ereader = (struct EbookReader){0};

  // We do not need to destroy gui here, cause it is destroyed in exit handler.
  // Thanks to cleaning up in exit handler we can be sure that on every `exit`
  // call all gui is cleanud up, so it won't leave display in some wierd state
  // like epaper powered on. Epaper if powered on for long time stops working.  
  cdk_errno = gui_init(&ereader.gui);
  CDK_TRY(cdk_errno);

  cdk_errno = gui_start(ereader.gui);
  CDK_TRY(cdk_errno);

  return 0;
  
error:
  log_error(cdk_errno);
  return cdk_errno->code;
}

static int configure_main(void) {
  puts(__func__);
  struct sigaction sa = {0};
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  int signals[] = {SIGINT, SIGTERM, SIGHUP};
  for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); i++) {
    if (sigaction(signals[i], &sa, NULL) == -1) {
      log_warn("Cannot sonfigure signal handler for %d", signals[i]);
      return 1;
    }
  }
  if (atexit(exit_handler) != 0) {
    log_warn("Cannot set exit handler");
    return EXIT_FAILURE;
  };

  return 2;
}

static void signal_handler(int signum) {
  puts(__func__);
  gui_panic(ereader.gui);
  _exit(1);
}

static void exit_handler(void) {
  puts(__func__);
  gui_destroy(&ereader.gui);
}
