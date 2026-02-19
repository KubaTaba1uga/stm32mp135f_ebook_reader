#include <stdio.h>
#include <string.h>

#include "book_settings/book_settings.h"
#include "book_settings/core.h"
#include "display/display.h"
#include "event_queue/event_queue.h"
#include "library/library.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"

enum BookSettingsStates {
  BookSettingsStates_NONE,
  BookSettingsStates_ACTIVE,
  BookSettingsStates_SET_SCALE,
  BookSettingsStates_SET_X_OFF,
  BookSettingsStates_SET_Y_OFF,
  BookSettingsStates_MAX,
};

struct BookSettings {
  enum BookSettingsStates current_state;
  event_queue_t evqueue;
  display_t display;
  library_t library;
  struct {
    struct BookSettingsView settings_view;
    struct BookSettingsSetScaleView set_scale_view;
    struct BookSettingsSetXOffView set_x_off_view;
    struct BookSettingsSetYOffView set_y_off_view;
    book_t book;
  } ctx;
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
static void book_click_set_scale_cb(void *data);
static void book_inc_scale_cb(void *data);
static void book_dec_scale_cb(void *data);
static void book_back_cb(void *data);
static void book_click_set_x_off_cb(void *data);
static void book_click_set_y_off_cb(void *data);
static void book_settings_activate_set_scale(enum Events __, ref_t ___,
                                             void *sub_data);
static void book_settings_deactivate_set_scale(enum Events __, ref_t ___,
                                               void *sub_data);
static void book_settings_close_set_scale(enum Events __, ref_t ___,
                                          void *sub_data);
static void book_settings_activate_set_x_off(enum Events __, ref_t ___,
                                             void *sub_data);
static void book_settings_inc_scale(enum Events __, ref_t ___, void *sub_data);
static void book_settings_dec_scale(enum Events __, ref_t ___, void *sub_data);
static void book_settings_close_settings(void *data);
static void book_settings_inc_x_off(enum Events __, ref_t ___, void *sub_data);
static void book_settings_dec_x_off(enum Events __, ref_t ___, void *sub_data);
static void book_settings_close_set_x_off(enum Events __, ref_t ___,
                                          void *sub_data);
static void book_settings_deactivate_set_x_off(enum Events __, ref_t ___,
                                               void *sub_data);
static void book_settings_activate_set_y_off(enum Events __, ref_t ___,
                                             void *sub_data);
static void book_settings_inc_y_off(enum Events __, ref_t ___, void *sub_data);
static void book_settings_dec_y_off(enum Events __, ref_t ___, void *sub_data);
static void book_settings_close_set_y_off(enum Events __, ref_t ___,
                                          void *sub_data);

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
                [Events_BTN_BOOK_SETTINGS_ENTER_SET_SCALE_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_SCALE,
                        .action = book_settings_activate_set_scale,
                    },
                [Events_BTN_BOOK_SETTINGS_ENTER_SET_X_OFF_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_X_OFF,
                        .action = book_settings_activate_set_x_off,
                    },
                [Events_BTN_BOOK_SETTINGS_ENTER_SET_Y_OFF_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_Y_OFF,
                        .action = book_settings_activate_set_y_off,
                    },

                [Events_BOOK_SETTINGS_CLOSED] =
                    {
                        .next_state = BookSettingsStates_NONE,
                        .action = book_settings_deactivate,
                    },
            },
        [BookSettingsStates_SET_SCALE] =
            {
                [Events_BTN_BOOK_SETTINGS_MORE_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_SCALE,
                        .action = book_settings_inc_scale,
                    },
                [Events_BTN_BOOK_SETTINGS_LESS_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_SCALE,
                        .action = book_settings_dec_scale,
                    },
                [Events_BTN_BOOK_SETTINGS_EXIT_CLICKED] =
                    {
                        .next_state = BookSettingsStates_NONE,
                        .action = book_settings_close_set_scale,
                    },
            },
        [BookSettingsStates_SET_X_OFF] =
            {
                [Events_BTN_BOOK_SETTINGS_MORE_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_X_OFF,
                        .action = book_settings_inc_x_off,
                    },
                [Events_BTN_BOOK_SETTINGS_LESS_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_X_OFF,
                        .action = book_settings_dec_x_off,
                    },
                [Events_BTN_BOOK_SETTINGS_EXIT_CLICKED] =
                    {
                        .next_state = BookSettingsStates_NONE,
                        .action = book_settings_close_set_x_off,
                    },
            },
        [BookSettingsStates_SET_Y_OFF] =
            {
                [Events_BTN_BOOK_SETTINGS_MORE_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_Y_OFF,
                        .action = book_settings_inc_y_off,
                    },
                [Events_BTN_BOOK_SETTINGS_LESS_CLICKED] =
                    {
                        .next_state = BookSettingsStates_SET_Y_OFF,
                        .action = book_settings_dec_y_off,
                    },
                [Events_BTN_BOOK_SETTINGS_EXIT_CLICKED] =
                    {
                        .next_state = BookSettingsStates_NONE,
                        .action = book_settings_close_set_y_off,
                    },
            },

};

err_t book_settings_init(book_settings_t *out, display_t display,
                         event_queue_t evqueue, library_t library) {

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
  case BookSettingsStates_SET_SCALE:
    book_settings_deactivate_set_scale(Events_NONE, NULL, book_settings);
    break;
  case BookSettingsStates_SET_X_OFF:
    book_settings_deactivate_set_x_off(Events_NONE, NULL, book_settings);
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

  puts(events_dump(event));
}

static const char *book_settings_state_dump(enum BookSettingsStates state) {
  static char *dumps[BookSettingsStates_MAX] = {
      [BookSettingsStates_NONE] = "book_settings_none",
      [BookSettingsStates_ACTIVE] = "book_settings_activated",
      [BookSettingsStates_SET_SCALE] = "book_settings_set_scale",
      [BookSettingsStates_SET_X_OFF] = "book_settings_set_x_off",
  };

  if (state < BookSettingsStates_NONE || state >= BookSettingsStates_MAX ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void book_settings_activate(enum Events __, ref_t book, void *sub_data) {

  book_settings_t book_settings = sub_data;

  err_o = book_settings_view_init(
      &book_settings->ctx.settings_view, book_click_set_scale_cb,
      book_settings_close_settings, book_click_set_x_off_cb,
      book_click_set_y_off_cb, book_settings);
  ERR_TRY(err_o);

  book_settings->ctx.book = mem_ref(book);
  display_add_to_ingroup(book_settings->display,
                         book_settings->ctx.settings_view.settings);

  return;

error_out:;
  // @todo post error event
};

static void book_settings_deactivate(enum Events __, ref_t ___,
                                     void *sub_data) {
  puts(__func__);
  book_settings_t book_settings = sub_data;

  display_del_from_ingroup(book_settings->display,
                           book_settings->ctx.settings_view.settings);

  book_settings_view_destroy(&book_settings->ctx.settings_view);

  mem_deref(book_settings->ctx.book);
  memset(&book_settings->ctx.settings_view, 0,
         sizeof(book_settings->ctx.settings_view));
};

static void book_click_set_scale_cb(void *data) {
  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue,
                   Events_BTN_BOOK_SETTINGS_ENTER_SET_SCALE_CLICKED,
                   book_settings->ctx.book);
}

static void book_click_set_x_off_cb(void *data) {
  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue,
                   Events_BTN_BOOK_SETTINGS_ENTER_SET_X_OFF_CLICKED,
                   book_settings->ctx.book);
}

static void book_settings_activate_set_scale(enum Events __, ref_t ___,
                                             void *sub_data) {
  book_settings_t book_settings = sub_data;

  err_o = book_settings_set_scale_view_init(
      &book_settings->ctx.set_scale_view,
      book_get_scale(book_settings->ctx.book), book_inc_scale_cb,
      book_dec_scale_cb, book_back_cb, book_settings);
  ERR_TRY(err_o);

  display_del_from_ingroup(book_settings->display,
                           book_settings->ctx.settings_view.settings);

  book_settings_view_destroy(&book_settings->ctx.settings_view);

  display_add_to_ingroup(book_settings->display,
                         book_settings->ctx.set_scale_view.set_scale);

  return;

error_out:;
  // @todo: post error to queue
}

static void book_inc_scale_cb(void *data) {
  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue,
                   Events_BTN_BOOK_SETTINGS_MORE_CLICKED,
                   book_settings->ctx.book);
}

static void book_dec_scale_cb(void *data) {
  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue,
                   Events_BTN_BOOK_SETTINGS_LESS_CLICKED,
                   book_settings->ctx.book);
}

static void book_settings_inc_scale(enum Events __, ref_t ___, void *sub_data) {

  book_settings_t book_settings = sub_data;
  double scale = book_get_scale(book_settings->ctx.book);
  scale += 0.25;
  book_set_scale(book_settings->ctx.book, scale);
  book_settings_set_scale_view_set_scale(&book_settings->ctx.set_scale_view,
                                         scale);

  event_queue_push(book_settings->evqueue, Events_BOOK_UPDATED,
                   book_settings->ctx.book);
}

static void book_settings_dec_scale(enum Events __, ref_t ___, void *sub_data) {

  book_settings_t book_settings = sub_data;
  double scale = book_get_scale(book_settings->ctx.book);
  scale -= 0.25;
  book_set_scale(book_settings->ctx.book, scale);
  book_settings_set_scale_view_set_scale(&book_settings->ctx.set_scale_view,
                                         scale);

  event_queue_push(book_settings->evqueue, Events_BOOK_UPDATED,
                   book_settings->ctx.book);
}

static void book_settings_deactivate_set_scale(enum Events __, ref_t ___,
                                               void *sub_data) {
  puts(__func__);
  book_settings_t book_settings = sub_data;

  book_settings_set_scale_view_destroy(&book_settings->ctx.set_scale_view);
  mem_deref(book_settings->ctx.book);
  memset(&book_settings->ctx.set_scale_view, 0,
         sizeof(book_settings->ctx.set_scale_view));
}

static void book_back_cb(void *data) {
  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue,
                   Events_BTN_BOOK_SETTINGS_EXIT_CLICKED,
                   book_settings->ctx.book);
}

static void book_settings_close_set_scale(enum Events __, ref_t ___,
                                          void *sub_data) {
  book_settings_t book_settings = sub_data;

  book_settings_deactivate_set_scale(__, ___, sub_data);

  event_queue_push(book_settings->evqueue, Events_BOOK_SETTINGS_CLOSED,
                   book_settings->ctx.book);
}

static void book_settings_close_settings(void *data) {
  puts(__func__);
  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue, Events_BOOK_SETTINGS_CLOSED,
                   book_settings->ctx.book);
}

static void book_settings_activate_set_x_off(enum Events __, ref_t ___,
                                             void *sub_data) {
  book_settings_t book_settings = sub_data;

  err_o = book_settings_set_x_off_view_init(
      &book_settings->ctx.set_x_off_view,
      book_get_x_off(book_settings->ctx.book), book_inc_scale_cb,
      book_dec_scale_cb, book_back_cb, book_settings);
  ERR_TRY(err_o);

  display_del_from_ingroup(book_settings->display,
                           book_settings->ctx.settings_view.settings);

  book_settings_view_destroy(&book_settings->ctx.settings_view);

  display_add_to_ingroup(book_settings->display,
                         book_settings->ctx.set_x_off_view.set_x_off);

  return;

error_out:;
  // @todo: post error to queue
}

static void book_settings_deactivate_set_x_off(enum Events __, ref_t ___,
                                               void *sub_data) {
  puts(__func__);
  book_settings_t book_settings = sub_data;

  book_settings_set_x_off_view_destroy(&book_settings->ctx.set_x_off_view);
  mem_deref(book_settings->ctx.book);
  memset(&book_settings->ctx.set_x_off_view, 0,
         sizeof(book_settings->ctx.set_x_off_view));
}

static void book_settings_inc_x_off(enum Events __, ref_t ___, void *sub_data) {
  book_settings_t book_settings = sub_data;
  int x_off = book_get_x_off(book_settings->ctx.book);
  x_off += 25;
  book_set_x_off(book_settings->ctx.book, x_off);
  book_settings_set_x_off_view_set_value(&book_settings->ctx.set_x_off_view,
                                         x_off);

  event_queue_push(book_settings->evqueue, Events_BOOK_UPDATED,
                   book_settings->ctx.book);
}

static void book_settings_dec_x_off(enum Events __, ref_t ___, void *sub_data) {
  book_settings_t book_settings = sub_data;
  int x_off = book_get_x_off(book_settings->ctx.book);
  x_off -= 25;
  book_set_x_off(book_settings->ctx.book, x_off);
  book_settings_set_x_off_view_set_value(&book_settings->ctx.set_x_off_view,
                                         x_off);

  event_queue_push(book_settings->evqueue, Events_BOOK_UPDATED,
                   book_settings->ctx.book);
}

static void book_settings_close_set_x_off(enum Events __, ref_t ___,
                                          void *sub_data) {
  book_settings_t book_settings = sub_data;

  book_settings_deactivate_set_x_off(__, ___, sub_data);

  event_queue_push(book_settings->evqueue, Events_BOOK_SETTINGS_CLOSED,
                   book_settings->ctx.book);
}

static void book_settings_activate_set_y_off(enum Events __, ref_t ___,
                                             void *sub_data) {
  book_settings_t book_settings = sub_data;

  err_o = book_settings_set_y_off_view_init(
      &book_settings->ctx.set_y_off_view,
      book_get_y_off(book_settings->ctx.book), book_inc_scale_cb,
      book_dec_scale_cb, book_back_cb, book_settings);
  ERR_TRY(err_o);

  display_del_from_ingroup(book_settings->display,
                           book_settings->ctx.settings_view.settings);

  book_settings_view_destroy(&book_settings->ctx.settings_view);

  display_add_to_ingroup(book_settings->display,
                         book_settings->ctx.set_y_off_view.set_y_off);

  return;

error_out:;
  // @todo: post error to queue
}

static void book_settings_deactivate_set_y_off(enum Events __, ref_t ___,
                                               void *sub_data) {
  puts(__func__);
  book_settings_t book_settings = sub_data;

  book_settings_set_y_off_view_destroy(&book_settings->ctx.set_y_off_view);
  mem_deref(book_settings->ctx.book);
  memset(&book_settings->ctx.set_y_off_view, 0,
         sizeof(book_settings->ctx.set_y_off_view));
}

static void book_click_set_y_off_cb(void *data) {
  book_settings_t book_settings = data;

  event_queue_push(book_settings->evqueue,
                   Events_BTN_BOOK_SETTINGS_ENTER_SET_Y_OFF_CLICKED,
                   book_settings->ctx.book);
}

static void book_settings_inc_y_off(enum Events __, ref_t ___, void *sub_data) {
  book_settings_t book_settings = sub_data;
  int y_off = book_get_y_off(book_settings->ctx.book);
  y_off += 25;
  book_set_y_off(book_settings->ctx.book, y_off);
  book_settings_set_y_off_view_set_value(&book_settings->ctx.set_y_off_view,
                                         y_off);

  event_queue_push(book_settings->evqueue, Events_BOOK_UPDATED,
                   book_settings->ctx.book);
}

static void book_settings_dec_y_off(enum Events __, ref_t ___, void *sub_data) {
  book_settings_t book_settings = sub_data;
  int y_off = book_get_y_off(book_settings->ctx.book);
  y_off -= 25;
  book_set_y_off(book_settings->ctx.book, y_off);
  book_settings_set_y_off_view_set_value(&book_settings->ctx.set_y_off_view,
                                         y_off);

  event_queue_push(book_settings->evqueue, Events_BOOK_UPDATED,
                   book_settings->ctx.book);
}

static void book_settings_close_set_y_off(enum Events __, ref_t ___,
                                          void *sub_data) {
  book_settings_t book_settings = sub_data;

  book_settings_deactivate_set_y_off(__, ___, sub_data);

  event_queue_push(book_settings->evqueue, Events_BOOK_SETTINGS_CLOSED,
                   book_settings->ctx.book);
}
