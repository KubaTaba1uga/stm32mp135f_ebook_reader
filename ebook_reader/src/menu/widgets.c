#include <lvgl.h>

#include "core/lv_obj.h"
#include "library/library.h"
#include "menu/core.h"
#include "utils/lvgl.h"
#include "utils/mem.h"
#include "utils/time.h"

typedef lvgl_obj_t wdgt_book_t;

struct WdgtBooks {
  void (*event_cb)(book_t, void *);
  void *event_data;

  lv_style_t *books_style;
  wdgt_book_t *books_arr;
  int books_arr_len;
};

struct WdgtBook {
  lv_obj_t *img;
  lv_obj_t *label;
  ref_t user_data;
};

static const int bar_y = 48;
static const int bar_clock_x = 336;
static const int books_x_off = 48;
static const int books_y_off = 64;
static const int book_x = 296;
static const int book_text_y = 80;
static const int book_y = 392 + book_text_y;

static wdgt_book_t wdgt_book_create(wdgt_books_t books, const char *book_title,
                                    bool is_focused, const uint8_t *thumbnail,
                                    ref_t data);
static void wdgt_book_destroy(wdgt_book_t book);
static void wdgt_book_event_cb(lv_event_t *e);

err_t wdgt_bar_init(wdgt_bar_t *out) {
  lv_obj_t *bar = lvgl_obj_create(lv_screen_active());
  lv_obj_set_size(bar, lv_display_get_horizontal_resolution(NULL), bar_y);
  lv_obj_set_pos(bar, 0, 0);

  lv_style_t *bar_style = mem_malloc(sizeof(lv_style_t));
  lv_style_init(bar_style);
  lv_style_set_border_color(bar_style, lv_color_black());
  lv_style_set_border_width(bar_style, 2);
  lv_style_set_border_opa(bar_style, LV_OPA_100);
  lv_style_set_border_side(bar_style,
                           (lv_border_side_t)(LV_BORDER_SIDE_BOTTOM));
  lv_style_set_pad_all(bar_style, 0);
  lv_obj_add_style(bar, bar_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  const int bar_clock_y = bar_y - 2;
  lv_obj_t *clock = lvgl_obj_create(bar);
  lv_obj_set_style_border_width(clock, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_pos(clock,
                 lv_display_get_horizontal_resolution(NULL) - bar_clock_x, 0);
  lv_obj_set_size(clock, bar_clock_x, bar_clock_y);
  lv_obj_set_user_data(bar, clock);

  lv_obj_t *clock_text = lv_label_create(clock);
  lv_obj_set_style_border_width(clock_text, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(clock_text, &lv_font_montserrat_30, 0);
  static char buf[100];
  char *res = time_now_dump(buf, sizeof(buf));
  assert(res != NULL);
  lv_label_set_text(clock_text, time_now_dump(buf, sizeof(buf)));
  lv_obj_set_user_data(clock, clock_text);
  lv_obj_set_user_data(clock_text, bar_style);

  lv_label_set_long_mode(clock_text, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_clear_flag(clock_text, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(clock, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

  *out = bar;

  return 0;
}

void wdgt_bar_destroy(wdgt_bar_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  lv_obj_t *clock = lv_obj_get_user_data(*out);
  lv_obj_t *clock_text = lv_obj_get_user_data(clock);
  lv_style_t *bar_style = lv_obj_get_user_data(clock_text);
  lv_obj_del(clock_text);
  lv_obj_del(clock);
  lv_style_reset(bar_style);
  mem_free(bar_style);
  lv_obj_del(*out);
  *out = NULL;
};

err_t wdgt_books_init(wdgt_books_t *out, books_list_t books,
                      void (*event_cb)(book_t, void *), void *event_data) {
  struct WdgtBooks *books_priv = mem_malloc(sizeof(struct WdgtBooks));
  lv_obj_t *books_container = *out = lvgl_obj_create(lv_screen_active());
  lv_gridnav_add(books_container, LV_GRIDNAV_CTRL_NONE);
  lv_obj_set_user_data(books_container, books_priv);

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

  lv_style_t *style = mem_malloc(sizeof(lv_style_t));
  lv_style_init(style);
  lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW_WRAP);
  lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_EVENLY);
  lv_style_set_layout(style, LV_LAYOUT_FLEX);
  lv_style_set_pad_column(style, 96);
  lv_style_set_pad_row(style, 48);
  lv_style_set_bg_color(style, lv_color_white());
  lv_obj_add_style(books_container, style, LV_PART_MAIN | LV_STATE_DEFAULT);

  wdgt_book_t *lv_books = mem_malloc(sizeof(wdgt_book_t) * books_list_len(books));
  wdgt_book_t lv_book = NULL;
  int i = 0;

  for (book_t book = books_list_get(books); book != NULL;
       book = books_list_get(books)) {
    lv_book = wdgt_book_create(
        books_container, book_get_title(book), lv_book == NULL,
        book_get_thumbnail(book, book_x, book_y - book_text_y), book);
    lv_obj_add_event_cb(lv_book, wdgt_book_event_cb, LV_EVENT_KEY, books_priv);

    lv_books[i++] = lv_book;
  }

  *books_priv = (struct WdgtBooks){
      .event_data = event_data,
      .books_arr = lv_books,
      .event_cb = event_cb,
      .books_style = style,
      .books_arr_len = i,
  };

  return 0;
}

void wdgt_books_destroy(wdgt_books_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  wdgt_books_t books = *out;
  struct WdgtBooks *wdgt = lv_obj_get_user_data(books);
  if (wdgt->books_arr) {
    for (int i = wdgt->books_arr_len - 1; i >= 0; i--) {
      if (wdgt->books_arr[i]) {
        wdgt_book_destroy(wdgt->books_arr[i]);
      }
    }
    mem_free(wdgt->books_arr);
    wdgt->books_arr = NULL;
  }

  lv_style_reset(wdgt->books_style);
  mem_free(wdgt->books_style);
  lv_obj_del(books);
  mem_free(wdgt);

  *out = NULL;
}

static wdgt_book_t wdgt_book_create(wdgt_books_t books, const char *book_title,
                             bool is_focused, const uint8_t *thumbnail,
                             ref_t data) {
  struct WdgtBook *wdgt = mem_malloc(sizeof(struct WdgtBook));
  lv_obj_t *book_card = lvgl_obj_create(books);
  lv_obj_t *book_img = NULL;

  lv_obj_set_size(book_card, book_x + 16, book_y + 16);
  lv_obj_set_user_data(book_card, wdgt);

  // Configure data required to display book
  if (thumbnail) {
    book_img = lv_image_create(book_card);
    lv_img_dsc_t *dsc = mem_malloc(sizeof(lv_image_dsc_t));
    *dsc = (lv_img_dsc_t){0};
    dsc->header.cf = LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;
    dsc->header.w = book_x;
    dsc->header.h = (book_y - book_text_y);
    dsc->data_size = dsc->header.w * dsc->header.h * 4;
    dsc->data = thumbnail;
    lv_image_set_src(book_img, dsc);
    lv_obj_set_style_border_width(book_img, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(book_img, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_user_data(book_img, dsc);
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

  *wdgt = (struct WdgtBook){
      .img = book_img,
      .label = book_label,
      .user_data = mem_ref(data),
  };

  return book_card;
}

static void wdgt_book_destroy(wdgt_book_t book) {
  struct WdgtBook *wdgt = lv_obj_get_user_data(book);
  void *book_img = lv_obj_get_user_data(wdgt->img);
  mem_deref(wdgt->user_data);
  lv_obj_del(wdgt->label);
  lv_obj_del(wdgt->img);
  mem_free(book_img);
  lv_obj_del(book);
  mem_free(wdgt);
};

static void wdgt_book_event_cb(lv_event_t *e) {
  wdgt_book_t wx = lv_event_get_current_target(e);
  struct WdgtBook *wdgt = lv_obj_get_user_data(wx);
  struct WdgtBooks *books = lv_event_get_user_data(e);
  lv_key_t key = lv_event_get_key(e);

  if (key == '\r' || key == '\n' || key == LV_KEY_ENTER) {
    books->event_cb(wdgt->user_data, books->event_data);
  }
}
