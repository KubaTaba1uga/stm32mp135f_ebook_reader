#include "menu/menu.h"
#include "display/display.h"
#include "event_bus/event_bus.h"
#include "utils/log.h"
#include "utils/mem.h"
#include <stdio.h>

enum MenuState {
  MenuState_NONE,
  MenuState_ACTIVE,
  MenuState_MAX,
};

struct Menu {
  enum MenuState current_state;
  display_t display;
};

struct MenuTransition {
  enum MenuState next_state;
  post_event_t action;
};

static void post_menu_event(struct Event event, void *data);
static void menu_activate(struct Event event, void *data);
static void menu_select_book(struct Event event, void *data);
static void menu_deactivate(menu_t menu);
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

err_t menu_init(menu_t *out) {
  menu_t menu = *out = mem_malloc(sizeof(struct Menu));
  *menu = (struct Menu){0};

  event_bus_register(BusEnum_MENU, post_menu_event, menu);

  return 0;
};

void menu_destroy(menu_t *out) {
  if (!out || !*out) {
    return;
  }

  event_bus_unregister(BusEnum_MENU, post_menu_event, *out);

  mem_free(*out);
  *out = NULL;
};

static void post_menu_event(struct Event event, void *data) {
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

static void menu_activate(struct Event event, void *data) { puts(__func__); }

static void menu_deactivate(menu_t menu) { puts(__func__); }

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

static void menu_select_book(struct Event event, void *data){}
