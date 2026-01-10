#include "gui/gui.h"
#include <lvgl.h>
#include <stdio.h>

#include "gui/gui_internal.h"
#include "utils/error.h"
#include "utils/mem.h"
#include "utils/time.h"
#include "utils/lvgl.h"

ebk_error_t ebk_gui_init(
    ebk_gui_t *out,
void (*input_callback)(enum ebk_GuiInputEventEnum event, void *data),
    void *input_data) {
  puts(__func__);

  ebk_lvgl_init();
  lv_tick_set_cb(ebk_time_now);

  ebk_gui_t gui = *out = ebk_mem_malloc(sizeof(struct ebk_Gui));
  *gui = (struct ebk_Gui){
      .inputh =
          {
              .callback = input_callback,
              .data = input_data,
          },
  };

  return 0;
}

int ebk_gui_tick(ebk_gui_t gui) {
  puts(__func__);
  return lv_timer_handler();
}

void ebk_gui_destroy(ebk_gui_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  ebk_lvgl_deinit();  
  ebk_mem_free(*out);
  *out = NULL;
}
