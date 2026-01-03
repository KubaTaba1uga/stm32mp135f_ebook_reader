#include <assert.h>
#include <lvgl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gui/lv/display/display.h"
#include "gui/lv/lv.h"
#include "lv_init.h"
#include "misc/lv_event.h"
#include "misc/lv_timer.h"
#include "misc/lv_types.h"
#include "utils/error.h"
#include "utils/log.h"
#include "utils/time.h"

static void lvgl_event_cb(lv_event_t *e);

cdk_error_t lvgl_init(lvgl_t out,
                      cdk_error_t (*callback)(enum LvglEvent event, void *data),
                      void *data) {
  lv_init();
  lv_tick_set_cb(time_now);

  out->callback = callback;
  out->callback_data = data;

  cdk_errno = lvgl_display_init(&out->display, lvgl_event_cb, out);
  CDK_TRY(cdk_errno);

  return 0;

error:
  lv_deinit();
  return cdk_errno;
}

void lvgl_destroy(lvgl_t out) {
  if (!out || !out->callback) {
    return;
  };
  
  lvgl_display_destroy(&out->display);
  lv_deinit();
};

uint32_t lvgl_process(lvgl_t out) { return lv_timer_handler(); }

static void lvgl_event_cb(lv_event_t *e) {
  lvgl_t lvgl = lv_event_get_user_data(e);
  lv_event_code_t code = lv_event_get_code(e);
  enum LvglEvent event;

  log_debug("Received lvgl event: %s", lv_event_code_get_name(code));

  switch (code) {
  case LV_EVENT_DELETE:
  case LV_EVENT_REFR_REQUEST:
    goto skip;
  default:
    log_warn("Unsupported event: %s", lv_event_code_get_name(code));
    goto skip; // TO-DO: change to goto error;
  }

  cdk_errno = lvgl->callback(event, lvgl->callback_data);
  CDK_TRY_CATCH(cdk_errno, error_dump);

  return;

error_dump:
  log_error(cdk_errno);
  /* error: */
  /* assert(false); */
skip:
  assert(true);
};
