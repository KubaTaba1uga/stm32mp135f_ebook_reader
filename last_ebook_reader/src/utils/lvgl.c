#include <lvgl.h>

static int ebk_lvgl_inits_done = 0;

void ebk_lvgl_init(void) {
  if (ebk_lvgl_inits_done == 0) {
    lv_init();
  }

  ebk_lvgl_inits_done++;
}

void ebk_lvgl_deinit(void) {
  ebk_lvgl_inits_done--;

  if (ebk_lvgl_inits_done == 0) {
    lv_deinit();
  }
};
