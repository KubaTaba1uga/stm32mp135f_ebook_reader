#include "display/lv_display.h"
#include "misc/lv_event.h"
#include "misc/lv_timer.h"
#include "misc/lv_types.h"
#include <stdio.h>
#if !EBK_DISPLAY_X11
#include "ui/display.h"
#include "utils/err.h"
err_t ui_display_x11_init(ui_display_t __, ui_t ___) {
  return err_errnos(EINVAL, "X11 is not supported!");
};
#else
#include "ui/display.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct UiDisplayX11 *ui_display_x11_t;

struct UiDisplayX11 {
  lv_obj_t *img;
  lv_img_dsc_t *dsc;
  lv_display_t *display;
  ui_t owner;
  int render_counter; // we use this counter to understand when image is
                      // rendered
};

static const int ui_display_x11_heigth = 800;
static const int ui_display_x11_width = 480;

static void ui_display_x11_destroy(ui_display_t);
static lv_display_t *ui_display_x11_get_lv_display(ui_display_t);
static err_t ui_display_x11_render(ui_display_t, unsigned char *);
static int ui_display_x11_get_render_size(ui_display_t);
static int ui_display_x11_get_render_x(ui_display_t);
static int ui_display_x11_get_render_y(ui_display_t);
static void ui_display_x11_destroy(ui_display_t);
static void ui_display_x11_cleanup(ui_display_x11_t);
static void ui_display_x11_render_event_cb(lv_event_t *);

err_t ui_display_x11_init(ui_display_t display, ui_t ui) {
  puts(__func__);
  ui_display_x11_t x11 = mem_malloc(sizeof(struct UiDisplayX11));
  *x11 = (struct UiDisplayX11){0};

  x11->display = lv_x11_window_create("ebook_reader", ui_display_x11_width,
                                      ui_display_x11_heigth);
  if (!x11->display) {
    err_errno = err_errnos(errno, "Cannot initialize X11 display");
    goto error_out;
  }
  lv_x11_inputs_create(x11->display, NULL);
  lv_display_set_default(x11->display);

  display->get_lv_display = ui_display_x11_get_lv_display;
  display->render = ui_display_x11_render;
  display->get_render_size = ui_display_x11_get_render_size;
  display->get_render_x = ui_display_x11_get_render_x;
  display->get_render_y = ui_display_x11_get_render_y;
  display->destroy = ui_display_x11_destroy;
  display->private = x11;

  return 0;

error_out:
  memset(display, 0, sizeof(struct UiDisplay));
  return err_errno;
};

static lv_display_t *ui_display_x11_get_lv_display(ui_display_t display) {
  ui_display_x11_t x11 = display->private;
  return x11->display;
}

static err_t ui_display_x11_render(ui_display_t display,
                                   unsigned char *render_buf) {
  ui_display_x11_t x11 = display->private;
  ui_display_x11_cleanup(x11);

  x11->img = lv_image_create(lv_screen_active());
  x11->dsc = mem_malloc(sizeof(lv_img_dsc_t));
  *x11->dsc = (lv_img_dsc_t){0};
  x11->dsc->header.cf = ui_display_color_format;
  x11->dsc->header.w = ui_display_x11_get_render_x(display);
  x11->dsc->header.h = ui_display_x11_get_render_y(display);
  x11->dsc->data_size = ui_display_x11_get_render_size(display);
  x11->dsc->data = render_buf;
  lv_image_set_src(x11->img, x11->dsc);
  lv_obj_set_pos(x11->img, 0, 0);
  lv_obj_set_size(x11->img, x11->dsc->header.w, x11->dsc->header.h);
  lv_display_add_event_cb(x11->display, ui_display_x11_render_event_cb,
                          LV_EVENT_ALL, x11);

  x11->render_counter = 0;
  while (x11->render_counter < 1) {
    lv_timer_handler();
  }

  lv_display_remove_event_cb_with_user_data(
      x11->display, ui_display_x11_render_event_cb, x11);

  return 0;
}
static int ui_display_x11_get_render_size(ui_display_t __) {
  return ((ui_display_x11_width + 7) / 8) * ui_display_x11_heigth;
}

static int ui_display_x11_get_render_x(ui_display_t __) {
  return ui_display_x11_width;
}

static int ui_display_x11_get_render_y(ui_display_t __) {
  return ui_display_x11_heigth;
}

static void ui_display_x11_destroy(ui_display_t display) {
  if (!display->private) {
    return;
  }

  ui_display_x11_t x11 = display->private;
  ui_display_x11_cleanup(x11);
  lv_display_delete(x11->display);
  mem_free(x11);
  display->private = NULL;
}

void ui_display_x11_render_cleanup(ui_display_t display) {
  ui_display_x11_t x11 = display->private;
  ui_display_x11_cleanup(x11);
};

static void ui_display_x11_cleanup(ui_display_x11_t x11) {
  if (x11->img) {
    lv_obj_delete(x11->img);
    x11->img = NULL;
  }
  if (x11->dsc) {
    mem_free((void *)x11->dsc->data);
    mem_free(x11->dsc);
    x11->dsc = NULL;
  }
}

static void ui_display_x11_render_event_cb(lv_event_t *lv_ev) {
  int code = lv_event_get_code(lv_ev);
  /* const char *lv_ev_name = lv_event_code_get_name(code); */
  /* puts(lv_ev_name); */

  ui_display_x11_t x11 = lv_event_get_user_data(lv_ev);
  if (code == LV_EVENT_REFR_READY) {
    x11->render_counter++;
  }
}

#endif
