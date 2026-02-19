#include <lvgl.h>

#include "lv_api_map_v8.h"
#include "reader/core.h"
#include "utils/err.h"
#include "utils/lvgl.h"
#include "utils/mem.h"

err_t wdgt_page_init(wdgt_page_t *out, const unsigned char *page_data,
                     int page_size, void (*event_cb)(lvgl_event_t),
                     void *event_data) {

  wdgt_page_t page = *out = lvgl_img_create(lv_screen_active());

  lv_img_dsc_t *dsc = mem_malloc(sizeof(lv_img_dsc_t));
  *dsc = (lv_img_dsc_t){0};
  dsc->header.cf = LV_COLOR_FORMAT_ARGB8888;
  dsc->header.w = lv_display_get_horizontal_resolution(NULL);
  dsc->header.h = lv_display_get_vertical_resolution(NULL);
  dsc->data_size = page_size;
  dsc->data = page_data;
  lv_image_set_src(page, dsc);
  lv_obj_set_user_data(page, dsc);

  lv_obj_add_event_cb(page, event_cb, LV_EVENT_KEY, event_data);

  return 0;
}

void wdgt_page_destroy(wdgt_page_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  lv_img_dsc_t *dsc = lv_obj_get_user_data(*out);
  mem_free(dsc);
  lv_obj_del(*out);
  *out = NULL;
}

void wdgt_page_refresh(wdgt_page_t page, const unsigned char *page_data,
                       int page_size) {
  lv_img_dsc_t *dsc = lv_obj_get_user_data(page);
  dsc->data = page_data;
  dsc->data_size = page_size;
  lv_image_set_src(page, dsc); // We need to set dsc again to let lvgl
                               //  now dsc got update.
}
