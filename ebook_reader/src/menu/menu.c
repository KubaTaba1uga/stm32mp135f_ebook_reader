#include <stdio.h>

#include "display/display.h"
#include "event_queue/event_queue.h"
#include "library/library.h"
#include "menu/core.h"
#include "menu/menu.h"
#include "utils/log.h"
#include "utils/mem.h"

enum MenuStates {
  MenuStates_NONE,
  MenuStates_ACTIVE,
  MenuStates_MAX,
};

struct Menu {
  enum MenuStates current_state;
  event_queue_t evqueue;
  books_list_t books;
  display_t display;
  library_t library;
  struct MenuView view;
};

struct MenuTransition {
  enum MenuStates next_state;
  post_event_func_t action;
};

static void menu_activate(enum Events event, ref_t event_data, void *sub_data);
static void select_book_cb(book_t, void *);
static void menu_deactivate(enum Events event, ref_t event_data, void *sub_data);
static void menu_post_event(enum Events event, ref_t event_data,
                            void *sub_data);
static const char *menu_state_dump(enum MenuStates state);

struct MenuTransition menu_fsm_table[MenuStates_MAX][Events_MAX] = {
    [MenuStates_NONE] =
        {
            [Events_BOOT_DONE] =
                {
                    .next_state = MenuStates_ACTIVE,
                    .action = menu_activate,
                },
        },
    [MenuStates_ACTIVE] =
        {
            [Events_BOOK_OPENED] =
                {
                    .next_state = MenuStates_NONE,
                    .action = menu_deactivate,
                },
        },

};

err_t menu_init(menu_t *out, display_t display, event_queue_t evqueue,
                library_t library) {
  puts(__func__);
  menu_t menu = *out = mem_malloc(sizeof(struct Menu));
  *menu = (struct Menu){.current_state = MenuStates_NONE,
                        .evqueue = evqueue,
                        .display = display,
                        .library = library};

  event_queue_register(evqueue, EventSubscribers_MENU, menu_post_event, menu);

  return 0;
};

void menu_destroy(menu_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  mem_free(*out);
  *out = NULL;
}

static void menu_activate(enum Events event, ref_t event_data, void *sub_data) {
  puts(__func__);
  menu_t menu = sub_data;

  menu->books = library_list_books(menu->library);

  err_o = menu_view_init(&menu->view, menu->books, select_book_cb, menu);
  ERR_TRY(err_o);

  display_add_to_ingroup(menu->display, menu->view.books);

  return;

error_out:
  mem_deref(menu->books);
  // @todo post error event
};

static void select_book_cb(book_t book, void *sub_data) {
  puts(__func__);
  menu_t menu = sub_data;

  event_queue_push(menu->evqueue, Events_BOOK_OPENED, book);
};

static void menu_deactivate(enum Events event, ref_t event_data, void *sub_data) {
  puts(__func__);
  menu_t menu = sub_data;

  mem_deref(menu->books);
  display_del_from_ingroup(menu->display, menu->view.books);
  menu_view_destroy(&menu->view);
};

static void menu_post_event(enum Events event, ref_t event_data,
                            void *sub_data) {
  puts(__func__);
  struct MenuTransition action;
  menu_t menu = sub_data;

  action = menu_fsm_table[menu->current_state][event];
  if (!action.action) {
    return;
  }

  if (menu->current_state != action.next_state) {
    log_info("%s -> %s", menu_state_dump(menu->current_state),
             menu_state_dump(action.next_state));
  }

  action.action(event, event_data, sub_data);
  menu->current_state = action.next_state;
}

static const char *menu_state_dump(enum MenuStates state) {
  static char *dumps[MenuStates_MAX] = {
      [MenuStates_NONE] = "menu_none",
      [MenuStates_ACTIVE] = "menu_activated",
  };

  if (state < MenuStates_NONE || state >= MenuStates_MAX || !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

