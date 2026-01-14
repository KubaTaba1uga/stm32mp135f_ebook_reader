#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "display/display.h"
#include "gui/gui.h"
#include "gui/gui_internal.h"
#include "gui/lvgl_wrapper.h"
#include "utils/error.h"
#include "utils/lvgl.h"
#include "utils/mem.h"
#include "utils/time.h"

enum ebk_GuiScreenEnum {
  ebk_GuiScreenEnum_BOOT,
  ebk_GuiScreenEnum_MENU,
  // Add new screns here
  ebk_GuiScreenEnum_MAX,
};

struct ebk_Gui {
  struct {
    void (*callback)(enum ebk_GuiInputEventEnum event, void *data);
    void *data;
  } inputh;

  lv_obj_t *bar;
  struct ebk_GuiMenu menu;
};

/* static lv_obj_t *ebk_gui_create_bar(ebk_gui_t gui, int h); */
static lv_obj_t *ebk_gui_create_book(ebk_book_t book, int is_current,
                                     lv_obj_t *table, int w, int h);
static void ebk_gui_event_cb(lv_event_t *e);

ebk_error_t ebk_gui_init(
    ebk_gui_t *out,
    void (*input_callback)(enum ebk_GuiInputEventEnum event, void *data),
    void *input_data) {
  puts(__func__);

  ebk_lvgl_init();
  lv_tick_set_cb(ebk_time_now);

  ebk_gui_t gui = *out = ebk_mem_malloc(sizeof(struct ebk_Gui));
  *gui = (struct ebk_Gui){
      .inputh =
          {
              .callback = input_callback,
              .data = input_data,
          },
  };

  /* lv_display_add_event_cb(lv_display_get_default(), ebk_gui_event_cb,
   * LV_EVENT_KEY, gui);   */

  return 0;
}

int ebk_gui_tick(ebk_gui_t gui) {
  puts(__func__);
  return lv_timer_handler();
}

void ebk_gui_destroy(ebk_gui_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  ebk_lvgl_deinit();
  ebk_mem_free(*out);
  *out = NULL;
}

/* void keyboard_read(lv_indev_t *indev, lv_indev_data_t *data) { */
/*   ebk_gui_t gui = data; */

/*   if(key_pressed()) { */
/*      data->key = my_last_key();            /\* Get the last pressed or
 * released key *\/ */
/*      data->state = LV_INDEV_STATE_PRESSED; */
/*   } else { */
/*      data->state = LV_INDEV_STATE_RELEASED; */
/*   } */
/* } */

/* lv_coord_t x = lv_obj_get_x(obj); */
/* lv_coord_t y = lv_obj_get_y(obj); */
/* lv_coord_t w = lv_obj_get_width(obj); */
/* lv_coord_t h = lv_obj_get_height(obj); */

ebk_error_t ebk_gui_menu_create(ebk_gui_t gui, ebk_books_list_t books,
                                int book_i, int *books_per_row) {
  gui->bar = gui->bar ? gui->bar : ebklv_bar_create();
  gui->menu.menu = gui->menu.menu ? gui->menu.menu : ebklv_menu_create();

  int32_t books_len = ebk_books_list_len(books);  
  lv_obj_t **lv_books = gui->menu.books =
      ebk_mem_malloc(sizeof(lv_obj_t *) * books_len);
  lv_obj_t *lv_book = NULL;
  int i = 0;
  for (ebk_book_t book = ebk_books_list_get(books); book != NULL;
       book = ebk_books_list_get(books)) {
    lv_book = lv_books[i++] =
        ebk_gui_create_book(book, lv_book == NULL, gui->menu.menu, 120, 230);
  }
  
  /* lv_obj_t *cont1 = lv_obj_create(lv_screen_active()); */
  /* lv_gridnav_add(cont1, LV_GRIDNAV_CTRL_NONE); */

  /* /\*Use flex here, but works with grid or manually placed objects as well*\/
   */
  /* lv_obj_set_flex_flow(cont1, LV_FLEX_FLOW_ROW_WRAP); */
  /* lv_obj_set_style_bg_color(cont1, lv_palette_lighten(LV_PALETTE_BLUE, 5), */
  /*                           LV_STATE_FOCUSED); */
  /* lv_obj_set_size(cont1, lv_pct(50), lv_pct(100)); */

  /* /\*Only the container needs to be in a group*\/ */
  /* lv_group_add_obj(lv_group_get_default(), cont1); */

  /* lv_obj_t *label = lv_label_create(cont1); */
  /* lv_label_set_text_fmt(label, "No rollover"); */

  /* uint32_t i; */
  /* for (i = 0; i < 10; i++) { */
  /*   lv_obj_t *obj = lv_button_create(cont1); */
  /*   lv_obj_set_size(obj, 70, LV_SIZE_CONTENT); */
  /*   lv_obj_add_flag(obj, LV_OBJ_FLAG_CHECKABLE); */
  /*   lv_group_remove_obj(obj); /\*Not needed, we use the gridnav instead*\/ */

  /*   label = lv_label_create(obj); */
  /*   lv_label_set_text_fmt(label, "%" LV_PRIu32 "", i); */
  /*   lv_obj_center(label); */

  /*   lv_obj_add_event_cb(obj, ebk_gui_event_cb, LV_EVENT_FOCUSED, gui); */
  /* } */

  return 0;
};

ebk_error_t ebk_gui_menu_create2(ebk_gui_t gui, ebk_books_list_t books,
                                 int book_i, int *books_per_row) {

  int bar_y = 30;
  /* gui->bar = ebk_gui_create_bar(gui, bar_y); */
  if (!gui->bar) {
    ebk_errno = ebk_errnos(ENODATA, "Cannot create `gui->bar`");
    goto error_out;
  }

  lv_coord_t menu_x_off = 15;
  lv_coord_t menu_y_off = 20;
  int menu_x = lv_display_get_horizontal_resolution(NULL) - menu_x_off;
  int menu_y =
      lv_display_get_vertical_resolution(NULL) - bar_y - menu_y_off * 2;
  gui->menu.menu = lv_obj_create(lv_screen_active());
  if (!gui->menu.menu) {
    ebk_errno = ebk_errnos(ENODATA, "Cannot create `gui->menu`");
    goto error_bar_cleanup;
  }
  // Delete borders from theme
  // Set menu at the beginning of display + bar
  lv_obj_set_pos(gui->menu.menu, menu_x_off,
                 bar_y + menu_y_off); // set offset 20, 20 for menu
  lv_obj_set_size(gui->menu.menu, menu_x, menu_y);
  lv_obj_set_style_pad_all(gui->menu.menu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(gui->menu.menu, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_t *books_table = gui->menu.menu_books_table =
      lv_obj_create(gui->menu.menu);
  if (!books_table) {
    ebk_errno =
        ebk_errnos(ENODATA, "Cannot create `gui->menu.menu_books_table`");
    goto error_menu_cleanup;
  }
  lv_obj_set_pos(books_table, 0, 0);
  lv_obj_set_size(books_table, menu_x, menu_y);

  int32_t books_len = ebk_books_list_len(books);
  int32_t *row_dsc = gui->menu.row_dsc =
      ebk_mem_malloc(sizeof(int32_t) * (books_len + 1));
  for (int32_t i = 0; i < books_len / 3; i++) {
    row_dsc[i] = 170 + 50;
  }
  row_dsc[books_len] = LV_GRID_TEMPLATE_LAST;

  int32_t *col_dsc = gui->menu.col_dsc =
      ebk_mem_malloc(sizeof(int32_t) * (books_len + 1));
  for (int32_t i = 0; i < 3; i++) {
    col_dsc[i] = 120;
  }
  col_dsc[books_len] = LV_GRID_TEMPLATE_LAST;

  lv_obj_set_grid_dsc_array(books_table, col_dsc, row_dsc);

  lv_coord_t books_table_x_pad = 20;
  lv_coord_t books_table_y_pad = 40;
  lv_obj_set_style_pad_row((lv_obj_t *)books_table, books_table_x_pad, 0);
  lv_obj_set_style_pad_column((lv_obj_t *)books_table, books_table_y_pad, 0);
  lv_obj_set_style_border_width(gui->menu.menu_books_table, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_t **lv_books = gui->menu.books =
      ebk_mem_malloc(sizeof(lv_obj_t *) * books_len);
  lv_obj_t *lv_book = NULL;
  int x = 0;
  int y = 0;
  int i = 0;
  for (ebk_book_t book = ebk_books_list_get(books); book != NULL;
       book = ebk_books_list_get(books)) {
    lv_book = lv_books[i++] =
        ebk_gui_create_book(book, lv_book == NULL, books_table, 120, 230);
    lv_obj_set_grid_cell(lv_book, LV_GRID_ALIGN_CENTER, x, 1,
                         LV_GRID_ALIGN_CENTER, y, 1);
    x++;
    if (x == 3) {
      x = 0;
      y++;
    }
  }
  /* lv_obj_t * cont1 = lv_obj_create(lv_screen_active()); */
  /* lv_gridnav_add(cont1, LV_GRIDNAV_CTRL_NONE); */

  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);

  for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i)) {
    if (lv_indev_get_type(i) == LV_INDEV_TYPE_KEYPAD) {
      lv_indev_set_group(i, g);
      break;
    }
  }

  lv_obj_add_event_cb(gui->menu.menu_books_table, ebk_gui_event_cb,
                      LV_EVENT_KEY, gui);
  lv_group_add_obj(g, gui->menu.menu_books_table);

  *books_per_row = 3;

  return 0;

error_menu_cleanup:
  lv_obj_delete(gui->menu.menu);
  gui->menu.menu = NULL;
error_bar_cleanup:
  lv_obj_delete(gui->bar);
  gui->bar = NULL;
error_out:
  return ebk_errno;
};

void ebk_gui_menu_destroy(ebk_gui_t gui) {
  if (!gui) {
    return;
  }
  if (gui->menu.menu) {
    lv_obj_delete(gui->menu.menu);
    gui->menu.menu = NULL;
  }
  if (gui->menu.menu_books_table) {
    lv_obj_delete(gui->menu.menu_books_table);
    gui->menu.menu_books_table = NULL;
  }
};

static lv_obj_t *ebk_gui_create_book(ebk_book_t book, int is_current,
                                     lv_obj_t *table, int w, int h) {
  lv_obj_t *book_card = ebklv_obj_create(table);
  int text_h = 50;
  lv_obj_set_size(book_card, w, h);

  /* lv_obj_t *book_img = lv_image_create(book_card); */
  /* lv_img_dsc_t *dsc = ebk_mem_malloc(sizeof(lv_img_dsc_t)); */
  /* *dsc   = (lv_img_dsc_t){0}; */
  /* dsc->header.cf = LV_COLOR_FORMAT_A1; */
  /* dsc->header.w = w; */
  /* dsc->header.h = (h - text_h) ; */
  /* dsc->data_size = ((w + 7) / 8) * (h - text_h) ; */
  /* dsc->data = (const uint8_t *)ebk_book_create_thumbnail(book, w, h -
   * text_h); */

  /* lv_image_set_src(book_img, dsc); */
  /* lv_obj_set_pos(book_img, 0, 0); */
  /* lv_obj_set_size(book_img, w, h - text_h); */

  /* lv_obj_t *book_img = ebklv_obj_create(book_card); */

  lv_obj_t *book_label = lv_label_create(book_card);
  lv_obj_set_pos(book_label, 0, h - text_h);

  const char *book_title = ebk_book_get_title(book);
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_black(),
                              LV_PART_MAIN);
  lv_label_set_text(book_label, book_title);

  if (is_current) {
    puts("HIT");
    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow, 15);
    lv_style_set_shadow_spread(&style_shadow, 10);
    lv_style_set_shadow_color(&style_shadow, lv_color_black());
    lv_obj_add_style(book_card, &style_shadow, 0);
    lv_obj_set_style_text_decor(book_label, LV_TEXT_DECOR_UNDERLINE,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
  }

  return book_card;
}

ebk_error_t ebk_gui_menu_select(ebk_gui_t gui, int book_i) {
  puts("HIT");
  static lv_style_t style_shadow;
  lv_obj_t *book_label = lv_obj_get_child(gui->menu.books[book_i], 1);
  lv_style_init(&style_shadow);
  lv_style_set_shadow_width(&style_shadow, 15);
  lv_style_set_shadow_spread(&style_shadow, 10);
  lv_style_set_shadow_color(&style_shadow, lv_color_black());
  lv_obj_add_style(gui->menu.books[book_i], &style_shadow, 0);
  lv_obj_set_style_text_decor(book_label, LV_TEXT_DECOR_UNDERLINE,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
  return 0;
}

static void ebk_gui_event_cb(lv_event_t *e) {
  ebk_gui_t gui = lv_event_get_user_data(e);
  lv_event_code_t code = lv_event_get_code(e);

  const char *name = lv_event_code_get_name(code);

  (void)gui;
  (void)code;
  printf("LV EVENT: %s\n", name);
};
