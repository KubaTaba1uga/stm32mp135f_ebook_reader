#include <lvgl.h>
#include <stdio.h>

#include "book/book.h"
#include "gui/lvgl_wrapper.h"
#include "core/lv_obj.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_style.h"
#include "core/lv_obj_style_gen.h"
#include "core/lv_obj_tree.h"
#include "gui/gui.h"
#include "gui/gui_internal.h"
#include "layouts/grid/lv_grid.h"
#include "lv_api_map_v8.h"
#include "misc/lv_color.h"
#include "misc/lv_palette.h"
#include "misc/lv_types.h"
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
  lv_obj_t *menu;
  lv_obj_t *menu_books_table;
};

static lv_obj_t *ebk_gui_create_bar(ebk_gui_t gui, int h);
static lv_obj_t *ebk_gui_create_book(ebk_book_t book, int is_current,
                                     lv_obj_t *table, int w, int h);

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

/* lv_coord_t x = lv_obj_get_x(obj); */
/* lv_coord_t y = lv_obj_get_y(obj); */
/* lv_coord_t w = lv_obj_get_width(obj); */
/* lv_coord_t h = lv_obj_get_height(obj); */

ebk_error_t ebk_gui_menu_create(ebk_gui_t gui, ebk_books_list_t books,
                                int book_i, int *books_per_row) {

  int bar_y = 30;
  gui->bar = ebk_gui_create_bar(gui, bar_y);
  if (!gui->bar) {
    ebk_errno = ebk_errnos(ENODATA, "Cannot create `gui->bar`");
    goto error_out;
  }

  lv_coord_t menu_x_off = 20;
  lv_coord_t menu_y_off = 20;
  int menu_x = lv_display_get_horizontal_resolution(NULL) - menu_x_off * 2;
  int menu_y =
      lv_display_get_vertical_resolution(NULL) - bar_y - menu_y_off * 2;
  gui->menu = lv_obj_create(lv_screen_active());
  if (!gui->menu) {
    ebk_errno = ebk_errnos(ENODATA, "Cannot create `gui->menu`");
    goto error_bar_cleanup;
  }
  // Delete borders from theme
  // Set menu at the beginning of display + bar
  lv_obj_set_pos(gui->menu, menu_x_off,
                 bar_y + menu_y_off); // set offset 20, 20 for menu
  lv_obj_set_size(gui->menu, menu_x, menu_y);
  lv_obj_set_style_pad_all(gui->menu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(gui->menu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  static int32_t row_dsc[] = {170 + 50, 170 + 50, 170 + 50,
                              LV_GRID_TEMPLATE_LAST};
  static int32_t col_dsc[] = {120, 120, 120, LV_GRID_TEMPLATE_LAST};
  lv_obj_t *books_table = gui->menu_books_table = lv_obj_create(gui->menu);
  if (!books_table) {
    ebk_errno = ebk_errnos(ENODATA, "Cannot create `gui->menu_books_table`");
    goto error_menu_cleanup;
  }

  lv_obj_set_pos(books_table, 0, 0);
  lv_obj_set_size(books_table, menu_x, menu_y);
  lv_obj_set_grid_dsc_array(books_table, col_dsc, row_dsc);

  lv_coord_t books_table_x_pad = 20;
  lv_coord_t books_table_y_pad = 40;
  lv_obj_set_style_pad_row((lv_obj_t *)books_table, books_table_x_pad, 0);
  lv_obj_set_style_pad_column((lv_obj_t *)books_table, books_table_y_pad, 0);
  lv_obj_set_style_border_width(gui->menu_books_table, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  
  ebk_book_t book = ebk_books_list_get(books);
  if (!book) {
    goto error_out;
  }

  lv_obj_t *lv_book = NULL;
  
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {    
      lv_book =
          ebk_gui_create_book(book, lv_book == NULL, books_table, 120, 230);
      lv_obj_set_grid_cell(lv_book, LV_GRID_ALIGN_CENTER, x, 1,
                           LV_GRID_ALIGN_CENTER, y, 1);
    }
  }

  (void)ebk_gui_create_book;

  return 0;

error_menu_cleanup:
  lv_obj_delete(gui->menu);
  gui->menu = NULL;
error_bar_cleanup:
  lv_obj_delete(gui->bar);
  gui->bar = NULL;
error_out:
  return ebk_errno;
};

void ebk_gui_menu_destroy(ebk_gui_t gui) {
  if (gui->menu) {
    lv_obj_delete(gui->menu);
    gui->menu = NULL;
  }
  if (gui->menu_books_table) {
    lv_obj_delete(gui->menu_books_table);
    gui->menu_books_table = NULL;
  }
};

static lv_obj_t *ebk_gui_create_book(ebk_book_t book, int is_current,
                                     lv_obj_t *table, int w, int h) {
  lv_obj_t *book_card = ebklv_obj_create(table);
  int text_h = 50;
  lv_obj_set_size(book_card, w, h);
  lv_obj_t *book_img = ebklv_obj_create(book_card);
  lv_obj_set_pos(book_img, 0, 0);
  lv_obj_set_size(book_img, w, h - text_h);

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

/**
   @todo Finish bar;
 */
static lv_obj_t *ebk_gui_create_bar(ebk_gui_t gui, int h) {
  gui->bar = ebklv_obj_create(lv_screen_active());
  lv_obj_set_size(gui->bar, lv_display_get_horizontal_resolution(NULL), h);
  lv_obj_set_pos(gui->bar, 0, 0);

  lv_obj_set_style_border_width(gui->bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_pad_all(gui->bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

  return gui->bar;
}

