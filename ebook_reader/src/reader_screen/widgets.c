#include <lvgl.h>
#include <stdio.h>

#include "reader_screen/core.h"

wx_reader_t wx_reader_create(int page_len,
                                   const unsigned char *page_buf) {
  puts(__func__);
  lv_obj_t *page_wx = lv_image_create(lv_screen_active());

  static lv_img_dsc_t dsc = {0};
  dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
  dsc.header.w = lv_display_get_horizontal_resolution(NULL);
  dsc.header.h = lv_display_get_vertical_resolution(NULL);
  dsc.data_size = page_len;
  dsc.data = page_buf;
  lv_image_set_src(page_wx, &dsc);

  return page_wx;
}

void wx_reader_destroy(wx_reader_t reader) { lv_obj_del(reader); }
