#include "ui/display.h"
#include "ui/ui.h"
#include "utils/err.h"

#if !EBK_DISPLAY_X11
err_t ui_display_x11_init(ui_display_t __, ui_t ___) {
  return err_errnos(EINVAL, "X11 is not supported!");
};
#else

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

static const int ui_display_x11_color_format = LV_COLOR_FORMAT_I1;
static const int ui_display_x11_heigth = 800;
static const int ui_display_x11_width = 480;

static err_t ui_display_x11_render(void *, unsigned char *, uint32_t);
static void ui_display_x11_destroy(void *);
static void ui_display_x11_render_cleanup(void *);
static void ui_display_x11_render_event_cb(lv_event_t *);
static err_t ui_display_x11_input_create(void *);

err_t ui_display_x11_init(ui_display_t display, ui_t ui) {
  puts(__func__);

  ui_display_x11_t x11 = mem_malloc(sizeof(struct UiDisplayX11));
  *x11 = (struct UiDisplayX11){
      .display = display,
  };

  lv_display_t *lv_disp = lv_x11_window_create(
      "ebook_reader", ui_display_x11_width, ui_display_x11_heigth);
  if (!lv_disp) {
    err_o = err_errnos(errno, "Cannot initialize X11 display");
    goto error_out;
  }
  lv_x11_inputs_create(lv_display, NULL);

  *display = (struct UiDisplay){
      .destroy = ui_display_x11_destroy,
      .render = ui_display_x11_render,
      .lv_disp = lv_disp,
      .owner = x11,
      .ui = ui,
  };

  return 0;

error_x11_cleanup:
  lv_display_delete(lv_disp);
error_out:
  mem_free(x11);
  return err_o;
};

static err_t ui_display_x11_render(void *display, unsigned char *render_buf,
                                   uint32_t render_buf_len) {
  ui_display_x11_t x11 = display;
  lv_display_t *lv_obj = ui_display_get_lv_obj(x11->display);

  ui_display_x11_render_cleanup(display);
  x11->render.img = lv_image_create(lv_screen_active());
  x11->render.dsc = mem_malloc(sizeof(lv_img_dsc_t));
  *x11->render.dsc = (lv_img_dsc_t){
      .header = {.cf = ui_display_x11_color_format,
                 .w = ui_display_x11_width,
                 .h = ui_display_x11_heigth},
      .data_size = render_buf_len,
      .data = render_buf,
  };
  lv_image_set_src(x11->render.img, x11->render.dsc);
  lv_obj_set_pos(x11->render.img, 0, 0);
  lv_obj_set_size(x11->render.img, x11->render.dsc->header.w,
                  x11->render.dsc->header.h);

  lv_display_add_event_cb(lv_obj, ui_display_x11_render_event_cb,
                          LV_EVENT_REFR_READY, x11);

  x11->render.counter = 0;
  while (x11->render.counter < 1) {
    lv_timer_handler();
  }

  lv_display_remove_event_cb_with_user_data(
      lv_obj, ui_display_x11_render_event_cb, x11);

  return 0;
}

static void ui_display_x11_destroy(void *display) {
  if (!display) {
    return;
  }

  ui_display_x11_t x11 = display;
  ui_display_x11_render_cleanup(x11);
  /* lv_display_delete(x11->display); */ // Cleaning up display causes crash in
                                         // exit handler.
  mem_free(x11);
}

static void ui_display_x11_render_cleanup(void *display) {
  ui_display_x11_t x11 = display;
  if (x11->render.img) {
    lv_obj_delete(x11->render.img);
    x11->render.img = NULL;
  }
  if (x11->render.dsc) {
    mem_free((void *)x11->render.dsc->data);
    mem_free(x11->render.dsc);
    x11->render.dsc = NULL;
  }
};

static void ui_display_x11_render_event_cb(lv_event_t *lv_ev) {
  ui_display_x11_t x11 = lv_event_get_user_data(lv_ev);
  x11->render.counter++;
}

#endif
