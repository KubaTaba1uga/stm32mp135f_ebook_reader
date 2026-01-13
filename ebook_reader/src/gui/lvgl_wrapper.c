
#include <lvgl.h>

#define EBK_DEBUG_LVGL 1

lv_obj_t *ebklv_obj_create(void *parent) {
  
  lv_obj_t *obj = lv_obj_create(parent);

#ifdef EBK_DEBUG_LVGL
  static int color = 6000;
  color += 2000;
  lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
#endif

  return obj;  
  
  }
