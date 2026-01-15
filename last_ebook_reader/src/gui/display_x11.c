#include "core/lv_obj_pos.h"
#include "core/lv_obj_tree.h"
#include "display/lv_display.h"
#include "gui/display.h"
#include "gui/gui.h"
#include "misc/lv_color.h"
#include "utils/error.h"
#include "utils/lvgl.h"
#include "utils/mem.h"

typedef struct ebk_X11 *ebk_x11_t;

struct ebk_X11 {
  lv_obj_t *img;
  lv_img_dsc_t *dsc;
  lv_display_t *display;
  ebk_gui_t owner;
};

static const int ebk_x11_heigth = 800;
static const int ebk_x11_width = 480;

static void ebk_display_x11_destroy(ebk_gui_display_t);
static lv_display_t *ebk_display_x11_get_lv_display(ebk_gui_display_t);
static ebk_error_t ebk_display_x11_render(ebk_gui_display_t, unsigned char *);
static int ebk_display_x11_get_render_size(ebk_gui_display_t);
static int ebk_display_x11_get_render_x(ebk_gui_display_t);
static int ebk_display_x11_get_render_y(ebk_gui_display_t);
static int ebk_display_x11_get_color_format(ebk_gui_display_t);
static void ebk_display_x11_destroy(ebk_gui_display_t);
static void ebk_display_x11_cleanup(ebk_x11_t);
ebk_error_t ebk_gui_display_x11_init(ebk_gui_display_t display, ebk_gui_t gui) {

  puts(__func__);
  ebk_x11_t x11 = ebk_mem_malloc(sizeof(struct ebk_X11));
  *x11 = (struct ebk_X11){0};

  x11->display =
      lv_x11_window_create("ebook_reader", ebk_x11_width, ebk_x11_heigth);
  if (!x11->display) {
    ebk_errno = ebk_errnos(errno, "Cannot initialize X11 display");
    goto error_out;
  }
  lv_x11_inputs_create(x11->display, NULL);
  lv_display_set_default(x11->display);

  display->get_lv_display = ebk_display_x11_get_lv_display;
  display->render = ebk_display_x11_render;
  display->get_render_size = ebk_display_x11_get_render_size;
  display->get_render_x = ebk_display_x11_get_render_x;
  display->get_render_y = ebk_display_x11_get_render_y;
  display->destroy = ebk_display_x11_destroy;
  display->private = x11;

  return 0;

error_out:

  memset(display, 0, sizeof(struct ebk_GuiDisplay));
  return ebk_errno;

  return 0;
};

static lv_display_t *ebk_display_x11_get_lv_display(ebk_gui_display_t display) {
  ebk_x11_t x11 = display->private;
  return x11->display;
}

static ebk_error_t ebk_display_x11_render(ebk_gui_display_t display,
                                          unsigned char *render_buf) {
  ebk_x11_t x11 = display->private;
  ebk_display_x11_cleanup(x11);

  x11->img = lv_image_create(lv_screen_active());
  x11->dsc = ebk_mem_malloc(sizeof(lv_img_dsc_t));
  *x11->dsc = (lv_img_dsc_t){0};
  x11->dsc->header.cf = ebk_display_x11_get_color_format(display);
  x11->dsc->header.w = ebk_x11_width;
  x11->dsc->header.h = ebk_x11_heigth;
  x11->dsc->data_size = ebk_display_x11_get_render_size(display);
  x11->dsc->data = render_buf;
  lv_image_set_src(x11->img, x11->dsc);
  lv_obj_set_pos(x11->img, 0, 0);
  lv_obj_set_size(x11->img, ebk_x11_width, ebk_x11_heigth);

  return 0;
}
static int ebk_display_x11_get_render_size(ebk_gui_display_t __) {
  return ((ebk_x11_width + 7) / 8) * ebk_x11_heigth;
}

static int ebk_display_x11_get_color_format(ebk_gui_display_t __) {
  return LV_COLOR_FORMAT_A1;
};

static int ebk_display_x11_get_render_x(ebk_gui_display_t __) {
  return ebk_x11_width;
}

static int ebk_display_x11_get_render_y(ebk_gui_display_t __) {
  return ebk_x11_heigth;
}

static void ebk_display_x11_destroy(ebk_gui_display_t display) {
  if (!display->private) {
    return;
  }

  ebk_x11_t x11 = display->private;
  ebk_display_x11_cleanup(x11);
  lv_display_delete(x11->display);
  ebk_mem_free(x11);
  display->private = NULL;
}

static void ebk_display_x11_cleanup(ebk_x11_t x11) {
  if (x11->img) {
    lv_obj_delete(x11->img);
  }
  if (x11->dsc) {
    ebk_mem_free(x11->dsc);
  }
}
