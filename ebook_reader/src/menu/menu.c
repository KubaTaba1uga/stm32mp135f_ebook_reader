#include <assert.h>
#include <stdio.h>

#include "book/book.h"
#include "event_bus/event_bus.h"
#include "library/library.h"
#include "menu/menu.h"
#include "utils/log.h"
#include "utils/mem.h"

enum MenuState {
  MenuState_NONE,
  MenuState_ACTIVE,
  MenuState_MAX,
};

struct Menu {
  enum MenuState current_state;
  books_list_t books;
  library_t library;
};

struct MenuTransition {
  enum MenuState next_state;
  post_event_t action;
};

static void post_menu_event(struct Event event, void *data);
static void menu_activate(struct Event event, void *data);
static void menu_select_book(struct Event event, void *data);
/* static void menu_deactivate(menu_t menu); */
static const char *menu_state_dump(enum MenuState state);

static struct MenuTransition fsm_table[MenuState_MAX][EventEnum_MAX] = {
    [MenuState_NONE] =
        {
            [EventEnum_BOOT_COMPLETED] =
                {
                    .action = menu_activate,
                    .next_state = MenuState_ACTIVE,
                },
        },
    [MenuState_ACTIVE] =
        {
            [EventEnum_BTN_ENTER] =
                {
                    .action = menu_select_book,
                    .next_state = MenuState_NONE,
                },
        },
};

err_t menu_init(menu_t *out, library_t lib) {
  menu_t menu = *out = mem_malloc(sizeof(struct Menu));
  *menu = (struct Menu){
      .library = lib,
  };

  event_bus_register(EndpointEnum_MENU, post_menu_event, menu);

  return 0;
};

void menu_destroy(menu_t *out) {
  if (!out || !*out) {
    return;
  }

  event_bus_unregister(EndpointEnum_MENU, post_menu_event, *out);

  mem_free(*out);
  *out = NULL;
};

static void post_menu_event(struct Event event, void *data) {
  puts(__func__);
  struct MenuTransition action;
  menu_t menu = data;

  action = fsm_table[menu->current_state][event.event];
  if (!action.action) {
    return;
  }

  if (menu->current_state != action.next_state) {
    log_info("%s -> %s", menu_state_dump(menu->current_state),
             menu_state_dump(action.next_state));
  }

  action.action(event, data);
  menu->current_state = action.next_state;
}

static void menu_activate(struct Event event, void *data) {
  puts(__func__);
  menu_t menu = data;

  menu->books = library_list_books(menu->library);
  if (!menu->books) {
    puts("NO BOOKS");
  }

  event_bus_post_event(BusEnum_MENU, (struct Event){
                                         .event = EventEnum_MENU_ACTIVATED,
                                         .data = menu->books,
                                     });
}

static const char *menu_state_dump(enum MenuState state) {
  static char *dumps[] = {
      [MenuState_NONE] = "menu_none",
      [MenuState_ACTIVE] = "menu_activated",
  };

  if (state < MenuState_NONE || state >= MenuState_MAX || !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void menu_select_book(struct Event event, void *data) {
  int *current_book_i = event.data;
  menu_t menu = data;

  assert(current_book_i != NULL);
  assert(menu != NULL);

  event_bus_post_event(BusEnum_MENU, (struct Event){
                                         .event = EventEnum_MENU_DEACTIVATED,
                                         .data = NULL,
                                     });

  event_bus_post_event(BusEnum_MENU,
                       (struct Event){
                           .event = EventEnum_BOOK_OPENED,
                           .data = books_list_pop(menu->books, *current_book_i),
                       });
}
