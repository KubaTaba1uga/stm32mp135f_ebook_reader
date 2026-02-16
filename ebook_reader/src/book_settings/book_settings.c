#include <stdio.h>

#include "book_settings/book_settings.h"
#include "book_settings/core.h"
#include "display/display.h"
#include "event_queue/event_queue.h"
#include "library/library.h"
#include "utils/log.h"
#include "utils/mem.h"

enum BookSettingsStates {
  BookSettingsStates_NONE,
  BookSettingsStates_ACTIVE,
  BookSettingsStates_MAX,
};

struct BookSettings {
  enum BookSettingsStates current_state;
  event_queue_t evqueue;
  display_t display;
  library_t library;
  struct BookSettingsView view;
};

struct BookSettingsTransition {
  enum BookSettingsStates next_state;
  post_event_func_t action;
};

static void book_settings_activate(enum Events __, ref_t ___, void *sub_data);
static void book_settings_deactivate(enum Events __, ref_t ___, void *sub_data);
static void book_settings_post_event(enum Events event, ref_t event_data,
                                     void *sub_data);
static const char *book_settings_state_dump(enum BookSettingsStates state);
static void book_update_cb(book_t book, void *data);

struct BookSettingsTransition
    book_settings_fsm_table[BookSettingsStates_MAX][Events_MAX] = {
        [BookSettingsStates_NONE] =
            {
                [Events_BOOK_SETTINGS_OPENED] =
                    {
                        .next_state = BookSettingsStates_ACTIVE,
                        .action = book_settings_activate,
                    },
            },
        [BookSettingsStates_ACTIVE] =
            {
                [Events_BOOK_SETTINGS_CLOSED] =
                    {
                        .next_state = BookSettingsStates_NONE,
                        .action = book_settings_deactivate,
                    },
            },
};

err_t book_settings_init(book_settings_t *out, display_t display,
                         event_queue_t evqueue, library_t library) {
  puts(__func__);
  book_settings_t book_settings = *out =
      mem_malloc(sizeof(struct BookSettings));
  *book_settings =
      (struct BookSettings){.current_state = BookSettingsStates_NONE,
                            .evqueue = evqueue,
                            .display = display,
                            .library = library};

  event_queue_register(evqueue, EventSubscribers_BOOK_SETTINGS,
                       book_settings_post_event, book_settings);

  return 0;
};

void book_settings_destroy(book_settings_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  book_settings_t book_settings = *out;

  switch (book_settings->current_state) {
  case BookSettingsStates_ACTIVE:
    book_settings_deactivate(Events_NONE, NULL, book_settings);
    break;
  default:;
  }

  event_queue_deregister(book_settings->evqueue,
                         EventSubscribers_BOOK_SETTINGS);
  mem_free(*out);
  *out = NULL;
}

static void book_settings_post_event(enum Events event, ref_t event_data,
                                     void *sub_data) {
  puts(__func__);
  struct BookSettingsTransition action;
  book_settings_t book_settings = sub_data;

  action = book_settings_fsm_table[book_settings->current_state][event];
  if (!action.action) {
    return;
  }

  if (book_settings->current_state != action.next_state) {
    log_info("%s -> %s", book_settings_state_dump(book_settings->current_state),
             book_settings_state_dump(action.next_state));
  }

  action.action(event, event_data, sub_data);
  book_settings->current_state = action.next_state;
}

static const char *book_settings_state_dump(enum BookSettingsStates state) {
  static char *dumps[BookSettingsStates_MAX] = {
      [BookSettingsStates_NONE] = "book_settings_none",
      [BookSettingsStates_ACTIVE] = "book_settings_activated",
  };

  if (state < BookSettingsStates_NONE || state >= BookSettingsStates_MAX ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void book_settings_activate(enum Events __, ref_t book, void *sub_data) {
  puts(__func__);
  book_settings_t book_settings = sub_data;

  err_o = book_settings_view_init(&book_settings->view, book, book_update_cb,
                                  book_settings);
  ERR_TRY(err_o);

  display_add_to_ingroup(book_settings->display, book_settings->view.settings);

  return;

error_out:;
  // @todo post error event
};

/* static void select_book_cb(book_t book, void *sub_data) { */
/*   puts(__func__); */
/*   book_settings_t book_settings = sub_data; */

/*   event_queue_push(book_settings->evqueue, Events_BOOK_OPENED, book); */
/* }; */

static void book_settings_deactivate(enum Events __, ref_t ___,
                                     void *sub_data) {
  puts(__func__);
  /* book_settings_t book_settings = sub_data; */

  /* display_del_from_ingroup(book_settings->display,
   * book_settings->view.books); */
  /* book_settings_view_destroy(&book_settings->view); */
};

static void book_update_cb(book_t book, void *data) {
  puts(__func__);

  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue, Events_BOOK_UPDATED, book);
}
