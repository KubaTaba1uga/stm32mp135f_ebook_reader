#include <lvgl.h>
#include <assert.h>

#include "utils/lvgl.h"
#include "widgets/image/lv_image.h"

/* #define EBK_DEBUG_LVGL 1 */

lvgl_obj_t lvgl_obj_create(lvgl_obj_t parent){
  lv_obj_t *obj = lv_obj_create(parent);
  assert(obj != NULL);
  
#ifdef EBK_DEBUG_LVGL
  static int color = 6000;
  color += 2000;
  lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
#endif

  return obj;
}

lvgl_obj_t lvgl_img_create(lvgl_obj_t parent){
  lv_obj_t *obj = lv_image_create(parent);
  assert(obj != NULL);
  
#ifdef EBK_DEBUG_LVGL
  static int color = 6000;
  color += 2000;
  lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
#endif

  return obj;
  }
