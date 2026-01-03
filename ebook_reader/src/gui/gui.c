#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "gui/gui.h"
#include "gui/lv/lv.h"
#include "gui/view/view.h"
#include "utils/error.h"
#include "utils/memory.h"
#include "utils/time.h"

struct Gui {
  struct Lvgl lvgl;
  struct View view;
  bool in_progress;
};

static cdk_error_t gui_ev_callback(enum LvglEvent event, void *data);

cdk_error_t gui_init(gui_t *out) {
  if (!out) {
    return cdk_errnoi(EINVAL);
  }

  *out = mem_malloc(sizeof(struct Gui));
  cdk_errno = lvgl_init(&(*out)->lvgl, gui_ev_callback, *out);
  CDK_TRY(cdk_errno);

  cdk_errno = view_init(&(*out)->view);
  CDK_TRY(cdk_errno);

  return 0;

error:
  mem_free(*out);
  *out = NULL;
  return cdk_errno;
};

void gui_destroy(gui_t *out) {
  if (!out || !*out) {
    return;
  }

  view_destroy(&(*out)->view);
  lvgl_destroy(&(*out)->lvgl);
  mem_free(*out);
  *out = NULL;
};

cdk_error_t gui_start(gui_t gui) {
  uint32_t ms;
  gui->in_progress = true;

  while (gui->in_progress) {
    puts("Gui processing");
    ms = lvgl_process(&gui->lvgl);
    cdk_errno = time_sleep_ms(ms);
    CDK_TRY(cdk_errno);
  }

  return 0;

error:
  return cdk_errno;
};

void gui_stop(gui_t gui) {
    gui->in_progress = false;  
}

static cdk_error_t gui_ev_callback(enum LvglEvent event, void *data) {
  gui_t gui = data;

  switch (event) {
  case LvglEvent_APP_CLOSED:
    gui_stop(gui);
    break;
  default:
    cdk_errno = cdk_errnof(ENOENT, "Unsupported event: %d", event);
    goto error;
  }

  return 0;

error:
  return cdk_errno;
};
