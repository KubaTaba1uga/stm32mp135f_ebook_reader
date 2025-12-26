#include <stdint.h>

#include "display_driver.h"
#include "image.h"
#include "utils/mem.h"

dd_error_t dd_image_init(dd_image_t *img, unsigned char *data,
                         uint32_t data_len, struct dd_ImagePoint resolution) {
  *img = dd_malloc(sizeof(struct dd_Image));
  **img = (struct dd_Image){
      .data = data, .data_len = data_len, .resolution = resolution};

  return NULL;
}

void dd_image_destroy(dd_image_t *img) {
  if (!img || !*img) {
    return;
  }

  dd_free(*img);
  *img = NULL;
}
