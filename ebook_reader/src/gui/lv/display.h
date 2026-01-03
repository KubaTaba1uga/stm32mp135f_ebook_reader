#ifndef EBOOK_READER_LVGL_DISPLAY_H
#define EBOOK_READER_LVGL_DISPLAY_H
#include <lvgl.h>

#include "utils/error.h"
#include "gui/lv/x11.h"
#include "gui/lv/wvs7in5v2b.h"

struct LvglDisplay {
  lv_display_t *display;
#ifdef EBOOK_READER_X11_ENABLED
  struct LvglX11Driver x11_driver;
#endif
#ifdef EBOOK_READER_WVS7IN5V2B_ENABLED
  struct LvglWvs7in5V2bDriver wvs7in5v2b_driver;
#endif
};

typedef struct LvglDisplay *lvgl_display_t;

cdk_error_t lvgl_display_init(lvgl_display_t out, void (*callback)(lv_event_t *e), void*data);
void lvgl_display_destroy(lvgl_display_t);

#endif // EBOOK_READER_LVGL_DISPLAY_H
