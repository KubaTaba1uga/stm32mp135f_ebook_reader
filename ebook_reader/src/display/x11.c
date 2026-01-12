#ifdef EBK_DISPLAY_X11
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "display/display_internal.h"
#include "display/x11.h"
#include "utils/error.h"
#include "utils/lvgl.h"
#include "utils/mem.h"

typedef struct ebk_X11 *ebk_x11_t;

struct ebk_X11 {
  lv_display_t *x11_display;
};

static int ebk_x11_width = 480;
static int ebk_x11_heigth = 800;
static void ebk_display_x11_destroy(ebk_display_module_t);
static ebk_error_t ebk_display_x11_show_boot_img(ebk_display_module_t);
static ebk_error_t ebk_display_x11_show_menu(ebk_display_module_t, ebk_gui_t);

ebk_error_t ebk_display_x11_init(ebk_display_module_t module) {
  puts(__func__);

  ebk_x11_t x11 = ebk_mem_malloc(sizeof(struct ebk_X11));
  *x11 = (struct ebk_X11){0};

  module->destroy = ebk_display_x11_destroy;
  module->show_boot_img = ebk_display_x11_show_boot_img;
  module->show_menu = ebk_display_x11_show_menu;
  module->private = x11;

  ebk_lvgl_init();

  x11->x11_display =
      lv_x11_window_create("ebook_reader", ebk_x11_width, ebk_x11_heigth);
  if (!x11->x11_display) {
    ebk_errno = ebk_errnos(errno, "Cannot initialize X11 display");
    goto error_out;
  }

  lv_x11_inputs_create(x11->x11_display, NULL);

  return 0;

error_out:
  ebk_lvgl_deinit();
  memset(module, 0, sizeof(struct ebk_DisplayModule));
  return ebk_errno;
}

/**
   @todo Generate 24bit img for x11.
*/
static ebk_error_t ebk_display_x11_show_boot_img(ebk_display_module_t module) {
  puts(__func__);
  return 0;
};

static ebk_error_t ebk_display_x11_show_menu(ebk_display_module_t module,
                                             ebk_gui_t gui) {
  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Hello world");
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0x003a57),
                              LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  return 0;
}

static void ebk_display_x11_destroy(ebk_display_module_t module) {
  puts(__func__);
  if (!module || !module->private) {
    return;
  }

  ebk_lvgl_deinit();
  ebk_mem_free(module->private);
  module->private = NULL;
}
#else
#include "display/display_internal.h"
#include "utils/error.h"

ebk_error_t ebk_display_x11_init(ebk_display_module_t module) {
  ebk_errno = ebk_errnos(EINVAL, "X11 is not supported!");
  return ebk_errno;
}

#endif
