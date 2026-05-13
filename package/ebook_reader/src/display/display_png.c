#include <assert.h>
#include <errno.h>
#include <lvgl.h>
#include <math.h>
#include <stdio.h>

#include "cairo.h"
#include "display/display.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/time.h"
#include "utils/graphic.h"

struct Display {
  lv_group_t *lv_ingroup;
  lv_display_t *lv_disp;
  struct {
    unsigned char *buf;
    int len;
  } render;
};

static const int ui_display_it8951_heigth = 1872;
static const int ui_display_it8951_width = 1404; // Display is 1404 but lvgl in
                                                 // i1 needs byte aligned values
                                                 // 1404 % 8 != 0

static struct Trace display_trace = {0};

static void display_flush_callback(lv_display_t *display, const lv_area_t *area,
                                   uint8_t *px_map);

err_t display_init(display_t *out) {
  display_t display = *out = mem_malloc(sizeof(struct Display));
  *display = (struct Display){0};

  lv_tick_set_cb(time_now);
  
  display->lv_disp =
      lv_display_create(ui_display_it8951_width, ui_display_it8951_heigth);
  if (!display->lv_disp) {
    goto error_out;
  }

  // Configure LVGL
  lv_indev_t *lv_indev =
      lv_evdev_create(LV_INDEV_TYPE_KEYPAD, settings_input_path);
  lv_indev_set_display(lv_indev, display->lv_disp);
  display->lv_ingroup = lv_group_create();
  lv_indev_set_group(lv_indev, display->lv_ingroup);

  display->render.len =
      (      cairo_format_stride_for_width(cairo_color_format,
                                    ui_display_it8951_width) * ui_display_it8951_heigth);
  display->render.buf = mem_malloc(display->render.len);

  lv_display_set_color_format(display->lv_disp, lvgl_color_format);
  lv_display_set_user_data(display->lv_disp, display);
  lv_display_set_flush_cb(display->lv_disp, display_flush_callback);
  lv_display_set_buffers(display->lv_disp, display->render.buf, NULL,
                         display->render.len, LV_DISPLAY_RENDER_MODE_FULL);
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

void display_add_to_ingroup(display_t display, void *wx) {
  lv_group_add_obj(display->lv_ingroup, wx);
}

void display_del_from_ingroup(display_t _, void *wx) {
  lv_group_remove_obj(wx);
}

int display_get_x(display_t display) {
  return lv_display_get_horizontal_resolution(NULL);
};

int display_get_y(display_t display) {
  return lv_display_get_vertical_resolution(NULL);
}

void display_set_trace(void) { display_trace = trace_start("render display"); };

unsigned char *dd_wvs75v2b_rotate(int width, int heigth, unsigned char *buf,
                                  int buf_len) {
  unsigned char *dst = calloc(buf_len, 1);
  for (int y = 0; y < heigth; y++) {
    int src_row = y * width;
    for (int x = 0; x < width; x++) {
      int src_bit = src_row + (width - 1 - x);
      int dst_bit = x * heigth + y;
      int v = (buf[src_bit >> 3] >> (src_bit & 7)) & 1;
      if (v) {
        dst[dst_bit >> 3] |= (1 << (dst_bit & 7));
      }
    }
  }

  return dst;
}

static void display_flush_callback(lv_display_t *display, const lv_area_t *area,
                                   uint8_t *px_map) {
  puts(__func__);

  trace_end(&display_trace);

  char buf[128];
  static int i = 0;
  snprintf(buf, sizeof(buf), "screenshot_%d.png", i);
  i++;


  cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
      px_map, cairo_color_format, ui_display_it8951_width,
      ui_display_it8951_heigth,
      cairo_format_stride_for_width(cairo_color_format,
                                    ui_display_it8951_width));
  assert(cairo_surface != NULL);
  
  cairo_status_t cairo_status = cairo_surface_write_to_png(cairo_surface, buf);
  puts(cairo_status_to_string(cairo_status));
  printf("%s done\n", __func__);

  lv_display_flush_ready(display);

}

void display_panic(display_t display) {
  puts(__func__);
}


