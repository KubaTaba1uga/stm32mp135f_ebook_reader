#include "core/lv_group.h"
#include "display/lv_display_private.h"
#include "indev/lv_indev.h"
#if !EBK_DISPLAY_X11
#include "ui/display.h"
#include "ui/ui.h"
#include "utils/err.h"

err_t ui_display_x11_init(ui_display_t __, ui_t ___) {
  return err_errnos(EINVAL, "X11 is not supported!");
};
#else

#include <lvgl.h>

#include "ui/display.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct UiDisplayX11 *ui_display_x11_t;

struct UiDisplayX11 {
  struct {
    lv_obj_t *img;
    lv_img_dsc_t *dsc;
    int counter; // we use this counter to understand when image is
                 // rendered. Lvgl does not expose api for this.
  } render;
  ui_display_t display;
};

static const int ui_display_x11_heigth = 800;
static const int ui_display_x11_width = 480;

static void ui_display_x11_destroy(void *);

err_t ui_display_x11_init(ui_display_t display, ui_t ui) {
  ui_display_x11_t x11 = mem_malloc(sizeof(struct UiDisplayX11));
  *x11 = (struct UiDisplayX11){
      .display = display,
  };

  lv_display_t *lv_display = lv_x11_window_create(
      "ebook_reader", ui_display_x11_width, ui_display_x11_heigth);
  if (!lv_display) {
    err_o = err_errnos(errno, "Cannot initialize X11 display");
    goto error_out;
  }
  lv_x11_inputs_create(lv_display, NULL);
  

  *display = (struct UiDisplay){
      .lv_ingroup = lv_group_get_default(),
      .destroy = ui_display_x11_destroy,
      .lv_disp = lv_display,
      .owner = x11,
      .ui = ui,
  };

  return 0;

error_out:
  mem_free(x11);
  return err_o;
};

static void ui_display_x11_destroy(void *display) {
  if (!display) {
    return;
  }

  ui_display_x11_t x11 = display;
  mem_free(x11);
}

#endif
