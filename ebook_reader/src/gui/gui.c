#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "core/core_internal.h"
#include "core/lv_group.h"
#include "core/lv_obj_event.h"
#include "display/display.h"
#include "gui/gui.h"
#include "gui/gui_internal.h"
#include "gui/lvgl_wrapper.h"
#include "lv_api_map_v8.h"
#include "misc/lv_event.h"
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
    void (*callback)(enum ebk_GuiInputEventEnum event, void *data, void *arg);
    void *data;
  } inputh;

  lv_obj_t *bar;
  struct ebk_GuiMenu menu;
};

/* static void ebk_gui_event_cb(lv_event_t *e); */
static void ebk_gui_menu_book_event_cb(lv_event_t *e);

ebk_error_t
ebk_gui_init(ebk_gui_t *out,
             void (*input_callback)(enum ebk_GuiInputEventEnum event,
                                    void *data, void *arg),
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

ebk_error_t ebk_gui_menu_create(ebk_gui_t gui, ebk_books_list_t books,
                                int book_i, int *books_per_row) {
  gui->bar = gui->bar ? gui->bar : ebklv_bar_create();
  gui->menu.menu = ebklv_menu_create();
  gui->menu.books =
      ebk_mem_malloc(sizeof(lv_obj_t *) * ebk_books_list_len(books));

  lv_obj_t **lv_books = gui->menu.books;
  lv_obj_t *lv_book = NULL;
  int i = 0;

  { // Configure passing events to menu widget
    lv_group_t *group = gui->menu.group = lv_group_create();
    lv_group_set_default(group);

    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i)) {
      if (lv_indev_get_type(i) == LV_INDEV_TYPE_KEYPAD) {
        lv_indev_set_group(i, group);
        break;
      }
    }

    lv_group_add_obj(group, gui->menu.menu);
    lv_group_set_editing(group, false);
  }

  for (ebk_book_t book = ebk_books_list_get(books); book != NULL;
       book = ebk_books_list_get(books)) {
    lv_book = ebklv_menu_book_create(
        gui->menu.menu, ebk_book_get_title(book), lv_book == NULL,
        ebk_book_create_thumbnail(book, menu_book_x,
                                  menu_book_y - menu_book_text_y),
        i, gui);

    lv_obj_add_event_cb(lv_book, ebk_gui_menu_book_event_cb, LV_EVENT_KEY,
                        lv_book);

    lv_books[i++] = lv_book;
  }

  return 0;
};

void ebk_gui_menu_destroy(ebk_gui_t gui) { return; };

static void ebk_gui_menu_book_event_cb(lv_event_t *e) {
  ebklv_widget_menu_book_t book = lv_event_get_user_data(e);
  ebk_gui_t gui = ebklv_menu_book_get_gui(book);
  int id = ebklv_menu_book_get_id(book);

  if (lv_event_get_code(e) == LV_EVENT_KEY) {
    lv_key_t key = lv_event_get_key(e);
    if (key == '\r' || key == '\n') {
      key = LV_KEY_ENTER;
    }

    if (key == LV_KEY_ENTER) {
      gui->inputh.callback(ebk_GuiInputEventEnum_ENTER, gui->inputh.data, &id);
    }
  }
}
