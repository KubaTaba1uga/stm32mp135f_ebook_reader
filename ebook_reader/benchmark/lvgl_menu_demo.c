#include <cairo.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lv_init.h"
#include "misc/lv_color.h"
#include "misc/lv_timer.h"
#include "timing.h"

#include <lvgl.h>

#include "../src/db/db.h"
#include "../src/library/library.h"
#include "../src/menu/core.h"
#include "../src/utils/time.h"
#include "../src/utils/log.h"
#include "../src/utils/err.h"

#define ASSERT(x) if ((x) != 0){   log_error(err_o); exit(1); }

static void select_book_cb(book_t book, void *sub_data) {
  puts(__func__);
};


static void display_flush_callback(lv_display_t *display, const lv_area_t *area,
                                   uint8_t *px_map);
long long t0;
int x;
int y;
int cairo_format;
int lvgl_format;

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("%s <x> <y>\n", argv[0]);
    return 1;
  }

  x = atoi(argv[1]);
  y = atoi(argv[2]);
  cairo_format = CAIRO_FORMAT_ARGB32;
  lvgl_format = LV_COLOR_FORMAT_ARGB8888;

  printf("x=%d\n", x);
  printf("y=%d\n", y);

  lv_init();
  lv_tick_set_cb(time_now);

  lv_display_t *lv_disp = lv_display_create(x, y);
  if (!lv_disp) {
    return 1;
  }

  char *buf = malloc(cairo_format_stride_for_width(cairo_format, x) * y);
  lv_display_set_color_format(lv_disp, lvgl_format);
  lv_display_set_flush_cb(lv_disp, display_flush_callback);
  lv_display_set_buffers(lv_disp, buf, NULL,
                         cairo_format_stride_for_width(cairo_format, x) * y,
                         LV_DISPLAY_RENDER_MODE_FULL);
  
  db_t db;
  ASSERT(db_init(&db))  ;
  
  library_t lib;
  ASSERT(library_init(&lib, db))  ;

  books_list_t books = library_list_books(lib);
  ASSERT(books == 0);

  puts("START");
  
  t0 = now_ns();

  ASSERT(menu_view_init(&(struct MenuView) {0},  books, select_book_cb, NULL));
  while (1) {
    lv_timer_handler();
  }

  long long t1 = now_ns();
  print_ms("Cat rotated", (t1 - t0));

  return 0;
}

static void display_flush_callback(lv_display_t *display, const lv_area_t *area,
                                   uint8_t *px_map) {
  long long t1 = now_ns();
  print_ms("Cat rotated", (t1 - t0));

  printf("x=%d, y=%d, stride=%d\n", x, y,
         cairo_format_stride_for_width(cairo_format, x));

  cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
      px_map, cairo_format, x, y,
      cairo_format_stride_for_width(cairo_format, x));

  cairo_status_t cairo_status =
      cairo_surface_write_to_png(cairo_surface, "menu.png");
  puts(cairo_status_to_string(cairo_status));
  exit(0);
}

