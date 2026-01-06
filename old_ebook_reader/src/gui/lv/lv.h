#ifndef EBOOK_READER_LVGL_H
#define EBOOK_READER_LVGL_H
#include <lvgl.h>
#include <stdint.h>

#include "gui/lv/display/display.h"
#include "utils/error.h"

enum LvglEvent {
  LvglEvent_BUTTON_PRESSED,
  LvglEvent_APP_CLOSED,    
};

/**
 Lvgl is meant to serve as middleware between app's gui and the lvgl framework.
*/
struct Lvgl {
  struct LvglDisplay display;
  cdk_error_t (*callback)(enum LvglEvent event, void *data);
  void *callback_data;  
};

typedef struct Lvgl *lvgl_t;

cdk_error_t lvgl_init(lvgl_t out, cdk_error_t (*callback)(enum LvglEvent event, void *data),
                      void *data);
void lvgl_destroy(lvgl_t out);
uint32_t lvgl_process(lvgl_t out);
void lvgl_panic(lvgl_t out);
#endif // EBOOK_READER_LVGL_H
