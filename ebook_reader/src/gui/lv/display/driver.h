#ifndef EBOOK_READER_LVGL_DISPLAY_DRIVER_H
#define EBOOK_READER_LVGL_DISPLAY_DRIVER_H
#include <lvgl.h>
#include <stdint.h>

/**
   Role of Display driver is to provide generic interface for different displays
   that mey be used by the project.

   Matching the proper display driver, is about calling
   lvgl_display_driver_X_probe function if the display driver should be used
   probing function should return true and set appropriate ops for the driver.

 */
struct LvglDisplayDriver {
  void *data;
  lv_display_t *(*init)(uint32_t width, uint32_t heigth,
                        struct LvglDisplayDriver *out);
  void (*destroy)(struct LvglDisplayDriver *out);
};

typedef struct LvglDisplayDriver *lvgl_display_driver_t;

#endif // EBOOK_READER_LVGL_DISPLAY_DRIVER_H
