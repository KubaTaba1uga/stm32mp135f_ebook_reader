#include <assert.h>
#include <stdio.h>

#include "book_settings/book_settings.h"
#include "event_bus/event_bus.h"
#include "library/library.h"
#include "utils/log.h"
#include "utils/mem.h"

enum BookSettingsState {
  BookSettingsState_NONE,
  BookSettingsState_ACTIVE,
  BookSettingsState_MAX,
};

struct BookSettingsCtx {
  book_t book;
};

struct BookSettings {
  enum BookSettingsState current_state;
  bus_t bus;
  struct BookSettingsCtx ctx;
};

struct BookSettingsTransition {
  enum BookSettingsState next_state;
  post_event_t action;
};

static void post_book_settings_event(struct Event event, void *data);
static void book_settings_activate(struct Event event, void *data);
static void book_settings_deactivate(book_settings_t book_settings);
static void book_settings_select_setting(struct Event event, void *data);
static const char *book_settings_state_dump(enum BookSettingsState state);

static struct BookSettingsTransition
    fsm_table[BookSettingsState_MAX][EventEnum_MAX] = {
        [BookSettingsState_NONE] =
            {
                [EventEnum_BTN_ENTER] =
                    {
                        .action = book_settings_activate,
                        .next_state = BookSettingsState_ACTIVE,
                    },
            },
        [BookSettingsState_ACTIVE] =
            {
                [EventEnum_BTN_ENTER] =
                    {
                        .action = book_settings_select_setting,
                        .next_state = BookSettingsState_NONE,
                    },
            },
};

err_t book_settings_init(book_settings_t *out, bus_t bus) {
  book_settings_t book_settings = *out =
      mem_malloc(sizeof(struct BookSettings));
  *book_settings = (struct BookSettings){
      .bus = bus,
  };

  event_bus_register(bus, BusConnectorEnum_BOOK_SETTINGS,
                     post_book_settings_event, book_settings);
  (void)book_settings_deactivate;
  return 0;
};

void book_settings_destroy(book_settings_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  event_bus_unregister((*out)->bus, BusConnectorEnum_BOOK_SETTINGS,
                       post_book_settings_event, *out);

  mem_free(*out);
  *out = NULL;
};

static void book_settings_activate(struct Event event, void *data) {
  
  puts(__func__);

  book_settings_t book_settings = data;

  event_bus_post_event(
      book_settings->bus, BusEnum_BOOK_SETTINGS,
      (struct Event){.event = EventEnum_BOOK_SETTINGS_ACTIVATED,
                     .data = event.data});
}

static void post_book_settings_event(struct Event event, void *data) {
  puts(__func__);
  struct BookSettingsTransition action;
  book_settings_t book_settings = data;

  action = fsm_table[book_settings->current_state][event.event];
  if (!action.action) {
    return;
  }

  if (book_settings->current_state != action.next_state) {
    log_info("%s -> %s", book_settings_state_dump(book_settings->current_state),
             book_settings_state_dump(action.next_state));
  }

  action.action(event, data);
  book_settings->current_state = action.next_state;
}

static const char *book_settings_state_dump(enum BookSettingsState state) {
  static char *dumps[] = {
      [BookSettingsState_NONE] = "book_settings_none",
      [BookSettingsState_ACTIVE] = "book_settings_activated",
  };

  if (state < BookSettingsState_NONE || state >= BookSettingsState_MAX ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void book_settings_deactivate(book_settings_t book_settings) {}

static void book_settings_select_setting(struct Event event, void *data) {}
