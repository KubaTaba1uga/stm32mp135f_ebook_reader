#include <lvgl.h>

#include "display/display.h"
#include "core/lv_group.h"
#include "utils/mem.h"

struct Display {
  lv_group_t *lv_ingroup;
  lv_display_t *lv_disp;
};

static const int ui_display_x11_heigth = 1872;
static const int ui_display_x11_width = 1404;

err_t display_init(display_t *out) {
  display_t display = *out = mem_malloc(sizeof(struct Display));
  *display = (struct Display){0};

  lv_display_t *lv_display = lv_x11_window_create(
      "ebook_reader", ui_display_x11_width, ui_display_x11_heigth);
  if (!lv_display) {
    err_o = err_errnos(errno, "Cannot initialize X11 display");
    goto error_out;
  }
  lv_x11_inputs_create(lv_display, NULL);

  *display = (struct Display){
      .lv_ingroup = lv_group_get_default(),
      .lv_disp = lv_display,
  };

  return 0;

error_out:
  mem_free(display);
  return err_o;
}

void display_destroy(display_t *out) {
  if (!out || !*out) {
    return;
  }

  if ((*out)->lv_ingroup) {
    lv_group_delete((*out)->lv_ingroup);
  }

  mem_free(*out);
  *out = NULL;
}

void display_add_to_ingroup(display_t display, void *wx){
  lv_group_add_obj(display->lv_ingroup, wx);
}
