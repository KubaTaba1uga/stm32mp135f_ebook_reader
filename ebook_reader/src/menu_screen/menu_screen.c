#include "menu_screen/menu_screen.h"
#include "display/display.h"
#include "event_bus/event_bus.h"
#include "utils/mem.h"
#include "utils/log.h"
#include <stdio.h>

enum MenuScreenState {
  MenuScreenState_NONE,
  MenuScreenState_ACTIVE,
  MenuScreenState_MAX,
};

struct MenuScreen {
  enum MenuScreenState current_state;
  display_t display;
};

struct MenuScreenTransition {
  enum MenuScreenState next_state;
  post_event_t action;
};

static void post_menu_screen_event(struct Event event, void *data);
static void menu_screen_activate(struct Event event, void *data);
static void menu_screen_deactivate(struct Event event, void *data);
static const char *menu_screen_state_dump(enum MenuScreenState state);

static struct MenuScreenTransition
    fsm_table[MenuScreenState_MAX][EventEnum_MAX] = {
        [MenuScreenState_NONE] =
            {
                [EventEnum_MENU_ACTIVATED] =
                    {
                        .action = menu_screen_activate,
                        .next_state = MenuScreenState_ACTIVE,
                    },
            },
        [MenuScreenState_ACTIVE] =
            {
                [EventEnum_MENU_DEACTIVATED] =
                    {
                        .action = menu_screen_deactivate,
                        .next_state = MenuScreenState_NONE,
                    },
            },
};

err_t menu_screen_init(menu_screen_t *out, display_t display) {
  menu_screen_t mscreen = *out = mem_malloc(sizeof(struct MenuScreen));
  *mscreen = (struct MenuScreen){
      .display = display,
  };

  event_bus_register(BusEnum_MENU_SCREEN, post_menu_screen_event, mscreen);

  return 0;
};

void menu_screen_destroy(menu_screen_t *out) {
  if (!out || !*out) {
    return;
  }

  event_bus_unregister(BusEnum_MENU_SCREEN, post_menu_screen_event, *out);

  mem_free(*out);
  *out = NULL;
};

static void post_menu_screen_event(struct Event event, void *data) {
  struct MenuScreenTransition action;
  menu_screen_t mscreen = data;

  action = fsm_table[mscreen->current_state][event.event];
  if (!action.action) {
    return;
  }
  
  if (mscreen->current_state != action.next_state) {
    log_info("%s -> %s", menu_screen_state_dump(mscreen->current_state),
             menu_screen_state_dump(action.next_state));
  }

  action.action(event, data);
  mscreen->current_state = action.next_state;
}

static void menu_screen_activate(struct Event event, void *data) {
  puts(__func__);
}
static void menu_screen_deactivate(struct Event event, void *data) {
  puts(__func__);
}

static const char *menu_screen_state_dump(enum MenuScreenState state) {
  static char *dumps[] = {
      [MenuScreenState_NONE] = "menu_screen_none",
      [MenuScreenState_ACTIVE] = "menu_screen_activated",
  };

  if (state < MenuScreenState_NONE || state >= MenuScreenState_MAX || !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};
