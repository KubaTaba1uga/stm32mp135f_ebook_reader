#ifndef EBOOK_READER_LVGL_DISPLAY_H
#define EBOOK_READER_LVGL_DISPLAY_H
#include <lvgl.h>

#include "utils/error.h"
#include "gui/lv/display/driver.h"

struct LvglDisplay {
  lv_display_t *display;
  struct LvglDisplayDriver driver;  
};

typedef struct LvglDisplay *lvgl_display_t;

cdk_error_t lvgl_display_init(lvgl_display_t out, void (*callback)(lv_event_t *e), void*data);
void lvgl_display_destroy(lvgl_display_t);

void lvgl_display_panic(lvgl_display_t);
#endif // EBOOK_READER_LVGL_DISPLAY_H
