#include <lvgl.h>
#include <stdio.h>

#include "gui/gui.h"
#include "utils/error.h"
#include "utils/log.h"

struct EbookReader {
  gui_t gui;
};

static struct EbookReader ereader;

int main(void) {
  log_info("Hello world!");
  ereader = (struct EbookReader){0};

  cdk_errno = gui_init(&ereader.gui);
  CDK_TRY(cdk_errno);

  cdk_errno = gui_start(ereader.gui);
  CDK_TRY_CATCH(cdk_errno, error_gui_cleanup);
  
  return 0;

error_gui_cleanup:
  gui_destroy(&ereader.gui);
error:
  log_error(cdk_errno);
  return cdk_errno->code;
}
