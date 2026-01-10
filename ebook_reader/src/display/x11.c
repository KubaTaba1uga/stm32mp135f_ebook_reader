#include <stdio.h>

#include "display/display_internal.h"
#include "display/x11.h"
#include "utils/error.h"
#include "utils/mem.h"

#define EBK_ERR_FROM_DD(err)                                                   \
  ebk_errnos(dd_error_get_code(err), dd_error_get_msg(err))

typedef struct ebk_X11 *ebk_x11_t;

struct ebk_X11 {
  int dummy;
};

static void ebk_display_x11_destroy(ebk_display_module_t);

static ebk_error_t ebk_display_x11_show_boot_img(ebk_display_module_t);

ebk_error_t ebk_display_x11_init(ebk_display_module_t module) {
  puts(__func__);

  ebk_x11_t wvs = ebk_mem_malloc(sizeof(struct ebk_X11));
  *wvs = (struct ebk_X11){0};

  module->destroy = ebk_display_x11_destroy;
  module->show_boot_img = ebk_display_x11_show_boot_img;
  module->private = wvs;
  
  return 0;

}

/**
   @todo Generate 24bit img for x11.
*/
static ebk_error_t ebk_display_x11_show_boot_img(ebk_display_module_t module) {
  puts(__func__);
  return 0;
};
static void ebk_display_x11_destroy(ebk_display_module_t module) {
  puts(__func__);  
  if (!module || !module->private) {
    return;
  }

  ebk_mem_free(module->private);
  module->private = NULL;
}
