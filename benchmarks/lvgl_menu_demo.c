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

static int bar_y;
static int bar_clock_x;
static int books_x_off;
static int books_y_off;
static int book_x;
static int book_text_y;
static int book_y;

uint32_t time_now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

char *time_now_dump(char *buf, uint32_t buf_len) {
  struct tm *tmp;
  time_t t;

  t = time(NULL);
  tmp = localtime(&t);
  if (tmp == NULL) {
    return NULL;
  }

  if (strftime(buf, buf_len, "%a, %d %b %Y %H:%M", tmp) == 0) {
    return NULL;
  }

  return buf;
}

int wdgt_bar_init(void) {
  lv_obj_t *bar = lv_obj_create(lv_screen_active());
  lv_obj_set_size(bar, lv_display_get_horizontal_resolution(NULL), bar_y);
  lv_obj_set_pos(bar, 0, 0);

  static lv_style_t bar_style;
  lv_style_init(&bar_style);
  lv_style_set_border_color(&bar_style, lv_color_black());
  lv_style_set_border_width(&bar_style, 2);
  lv_style_set_border_opa(&bar_style, LV_OPA_100);
  lv_style_set_border_side(&bar_style,
                           (lv_border_side_t)(LV_BORDER_SIDE_BOTTOM));
  lv_style_set_pad_all(&bar_style, 0);
  lv_obj_add_style(bar, &bar_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  const int bar_clock_y = bar_y - 2;
  lv_obj_t *clock = lv_obj_create(bar);
  lv_obj_set_style_border_width(clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_pos(clock,
                 lv_display_get_horizontal_resolution(NULL) - bar_clock_x, 0);
  lv_obj_set_size(clock, bar_clock_x, bar_clock_y);
  lv_obj_set_user_data(bar, clock);

  lv_obj_t *clock_text = lv_label_create(clock);
  lv_obj_set_style_border_width(clock_text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(clock_text, &lv_font_montserrat_30, 0);
  static char buf[100];

  lv_label_set_text(clock_text, time_now_dump(buf, sizeof(buf)));
  lv_obj_set_user_data(clock, clock_text);
  lv_obj_set_user_data(clock_text, &bar_style);

  lv_label_set_long_mode(clock_text, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_clear_flag(clock_text, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(clock, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

  return 0;
}

static int wdgt_book_create(lv_obj_t *books, const char *book_title,
                            bool is_focused, const uint8_t *thumbnail);

int wdgt_books_init(void) {
  lv_obj_t *books_container = lv_obj_create(lv_screen_active());
  lv_gridnav_add(books_container, LV_GRIDNAV_CTRL_NONE);

  int books_x = lv_display_get_horizontal_resolution(NULL) - books_x_off * 2;
  int books_y = lv_display_get_vertical_resolution(NULL) - bar_y - books_y_off;

  lv_obj_set_pos(books_container, books_x_off / 2, bar_y + books_y_off / 2);
  lv_obj_set_size(books_container, books_x, books_y);
  lv_obj_set_style_pad_ver(books_container, books_y_off / 2,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_hor(books_container, books_x_off / 2,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(books_container, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_layout(&style, LV_LAYOUT_FLEX);
  lv_style_set_pad_column(&style, 96);
  lv_style_set_pad_row(&style, 48);
  lv_style_set_bg_color(&style, lv_color_white());
  lv_obj_add_style(books_container, &style, LV_PART_MAIN | LV_STATE_DEFAULT);

  for (int i = 0; i < 9; i++) {
    int buf_len = book_x * (book_y - book_text_y) * 4;
    uint8_t *buf = malloc(buf_len); // I8
    memset(buf, 0xFF, buf_len);

    wdgt_book_create(books_container, "Book", i == 0, buf);
  }

  return 0;
}

static int wdgt_book_create(lv_obj_t *books, const char *book_title,
                            bool is_focused, const uint8_t *thumbnail) {
  lv_obj_t *book_card = lv_obj_create(books);
  lv_obj_t *book_img = NULL;

  lv_obj_set_size(book_card, book_x + 16, book_y + 16);

  // Configure data required to display book
  if (thumbnail) {
    book_img = lv_image_create(book_card);
    static lv_img_dsc_t dsc = {0};
    dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    dsc.header.w = book_x;
    dsc.header.h = (book_y - book_text_y);
    dsc.data_size = dsc.header.w * dsc.header.h * 4;
    dsc.data = thumbnail;
    lv_image_set_src(book_img, &dsc);
    lv_obj_set_style_border_width(book_img, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(book_img, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  }

  // Configure book label
  lv_obj_t *book_label = lv_label_create(book_card);
  lv_obj_set_pos(book_label, 0, book_y - (book_text_y * 0.75));
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(),
                              LV_PART_MAIN);
  lv_obj_set_style_text_font(book_label, &lv_font_montserrat_24, 0);
  lv_label_set_text(book_label, book_title);

  // Configure not focused border
  lv_obj_set_style_border_width(book_card, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Configure focused border
  lv_obj_set_style_outline_width(book_card, 8, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_pad(book_card, 8, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_outline_color(book_card, lv_color_hex(0x00A0FF),
                                 LV_PART_MAIN | LV_STATE_FOCUSED);

  // Disable scrolling inside a card
  lv_label_set_long_mode(book_label, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_clear_flag(book_label, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(book_label, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_flag(book_card, LV_OBJ_FLAG_CLICK_FOCUSABLE);
  lv_obj_clear_flag(book_card, LV_OBJ_FLAG_SCROLLABLE);

  return 0;
}
void lv_example_get_started_1(void) {
  /*Change the active screen's background color*/
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), LV_PART_MAIN);

  /*Create a white label, set its text and align it to the center*/
  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Hello world");
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(),
                              LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void lv_example_line_1(void) {
  /*Create an array for the points of the line*/
  static lv_point_precise_t line_points[] = {
      {5, 5}, {70, 70}, {120, 10}, {180, 60}, {240, 10}};

  /*Create style*/
  static lv_style_t style_line;
  lv_style_init(&style_line);
  lv_style_set_line_width(&style_line, 8);
  lv_style_set_line_color(&style_line, lv_color_black());
  lv_style_set_line_rounded(&style_line, true);

  /*Create a line and apply the new style*/
  lv_obj_t *line1;
  line1 = lv_line_create(lv_screen_active());
  lv_line_set_points(line1, line_points, 5); /*Set the points*/
  lv_obj_add_style(line1, &style_line, 0);
  lv_obj_center(line1);
}

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

  if (x < 1000) {
    bar_y = 30;
    bar_clock_x = 160;
    books_x_off = 10;
    books_y_off = 20;
    book_x = 120;
    book_text_y = 50;
    book_y = 170 + book_text_y;
  } else {
    bar_y = 48;
    bar_clock_x = 336;
    books_x_off = 48;
    books_y_off = 64;
    book_x = 296;
    book_text_y = 80;
    book_y = 392 + book_text_y;
  }

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

  t0 = now_ns();

  /* lv_example_line_1(); */

  wdgt_bar_init();
  wdgt_books_init();
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
}
