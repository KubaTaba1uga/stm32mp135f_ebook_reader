#include <stdio.h>

#include "display/display_internal.h"
#include "utils/error.h"
#include "utils/mem.h"

#define EBK_ERR_FROM_DD(err)                                                   \
  ebk_errnos(dd_error_get_code(err), dd_error_get_msg(err))

typedef struct ebk_Wvs7in5V2b *ebk_wvs7in5v2b_t;

struct ebk_Wvs7in5V2b {
  dd_display_driver_t dd;
};

static void ebk_display_wvs7in5v2b_destroy(ebk_display_module_t);
static ebk_error_t ebk_display_wvs7in5v2b_show_boot_img(ebk_display_module_t);

ebk_error_t ebk_display_wvs7in5v2b_init(ebk_display_module_t module) {
  puts(__func__);

  ebk_wvs7in5v2b_t wvs = ebk_mem_malloc(sizeof(struct ebk_Wvs7in5V2b));
  *wvs = (struct ebk_Wvs7in5V2b){0};

  dd_error_t err = dd_display_driver_init(
      &wvs->dd, dd_DisplayDriverEnum_Wvs7in5V2b,
      &(struct dd_Wvs75V2bConfig){
          .dc = {.gpio_chip_path = "/dev/gpiochip8", .pin_no = 0},
          .rst = {.gpio_chip_path = "/dev/gpiochip2", .pin_no = 2},
          .bsy = {.gpio_chip_path = "/dev/gpiochip6", .pin_no = 3},
          .pwr = {.gpio_chip_path = "/dev/gpiochip0", .pin_no = 4},
          .spi = {.spidev_path = "/dev/spidev0.0"},
      });
  if (err) {
    ebk_errno = EBK_ERR_FROM_DD(err);
    goto error_out;
  }

  module->destroy = ebk_display_wvs7in5v2b_destroy;
  module->show_boot_img = ebk_display_wvs7in5v2b_show_boot_img;

  return 0;

error_out:
  return ebk_errno;
}

static ebk_error_t
ebk_display_wvs7in5v2b_show_boot_img(ebk_display_module_t module) {
  puts(__func__);
  return 0;
};

static void ebk_display_wvs7in5v2b_destroy(ebk_display_module_t module) {
  if (!module || !module->private) {
    return;
  }

  ebk_mem_free(module->private);
  module->private = NULL;
}
