#ifndef DISPLAY_DRIVER_IMAGE_H
#define DISPLAY_DRIVER_IMAGE_H
#include <stdint.h>

#include "display_driver.h"

struct dd_Image {
  unsigned char *data;
  uint32_t data_len;
  struct dd_ImagePoint resolution;
};

static inline unsigned char *dd_image_get_data(dd_image_t img) {
  return img->data;
}

static inline uint32_t dd_image_get_data_len(dd_image_t img) {
  return img->data_len;
}

static inline struct dd_ImagePoint *dd_image_get_resolution(dd_image_t img) {
  return &img->resolution;
}

#endif // DISPLAY_DRIVER_IMAGE_H
