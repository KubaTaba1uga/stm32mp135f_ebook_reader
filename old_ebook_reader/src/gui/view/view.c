#include "utils/error.h"
#include "gui/view/view.h"
#include <lvgl.h>

cdk_error_t view_init(view_t view) {
    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    return 0;
};

void view_destroy(view_t view){

};
