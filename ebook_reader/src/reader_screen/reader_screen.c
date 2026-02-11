#include <assert.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "core/lv_group.h"
#include "core/lv_obj.h"
#include "core/lv_obj_event.h"
#include "display/display.h"
#include "event_bus/event_bus.h"
#include "library/library.h"
#include "misc/lv_event.h"
#include "reader_screen/core.h"
#include "reader_screen/reader_screen.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"

enum ReaderScreenState {
  ReaderScreenState_NONE,
  ReaderScreenState_ACTIVE,
  ReaderScreenState_ON_HOLD,
  ReaderScreenState_MAX,
};

struct ReaderScreenCtx {
  wx_reader_t reader;
  book_t book;
  int x_off;
  int y_off;
  int scale;
  int page;
};

struct ReaderScreen {
  enum ReaderScreenState current_state;
  struct ReaderScreenCtx ctx;
  display_t display;
  bus_t bus;
};

struct ReaderScreenTransition {
  enum ReaderScreenState next_state;
  post_event_t action;
};

static void post_reader_screen_event(struct Event event, void *data);
static void reader_screen_activate(struct Event event, void *data);
static void reader_screen_deactivate(struct Event event, void *data);
static void reader_screen_refresh(struct Event event, void *data);
static const char *reader_screen_state_dump(enum ReaderScreenState state);
static void reader_screen_event_cb(lv_event_t *e);
static void reader_screen_put_on_hold(struct Event event, void *data);
static void reader_screen_resume(struct Event event, void *data);

static struct ReaderScreenTransition
    fsm_table[ReaderScreenState_MAX][EventEnum_MAX] = {
        [ReaderScreenState_NONE] =
            {
                [EventEnum_BOOK_OPENED] =
                    {
                        .action = reader_screen_activate,
                        .next_state = ReaderScreenState_ACTIVE,
                    },
            },
        [ReaderScreenState_ACTIVE] = {
            [EventEnum_BOOK_UPDATED] =
                {
                    .action = reader_screen_refresh,
                    .next_state = ReaderScreenState_ACTIVE,
                },
            [EventEnum_BOOK_CLOSED] =
                {
                    .action = reader_screen_deactivate,
                    .next_state = ReaderScreenState_NONE,
                },
            [EventEnum_BOOK_SETTINGS_ACTIVATED] =
                {
                    .action = reader_screen_put_on_hold,
                    .next_state = ReaderScreenState_ON_HOLD,
                },
            [EventEnum_BOOK_SETTINGS_DEACTIVATED] =
                {
                    .action = reader_screen_resume,
                    .next_state = ReaderScreenState_ACTIVE,
                },
            [EventEnum_BTN_MENU] =
                {
                    .action = reader_screen_deactivate,
                    .next_state = ReaderScreenState_NONE,
                },            
            },
};

err_t reader_screen_init(reader_screen_t *out, display_t display, bus_t bus) {
  reader_screen_t rscreen = *out = mem_malloc(sizeof(struct ReaderScreen));
  *rscreen = (struct ReaderScreen){
      .display = display,
      .bus = bus,
  };

  event_bus_register(bus, BusConnectorEnum_READER_SCREEN,
                     post_reader_screen_event, rscreen);

  return 0;
};

void reader_screen_destroy(reader_screen_t *out) {
  if (!out || !*out) {
    return;
  }

  switch ((*out)->current_state) {
  case ReaderScreenState_ACTIVE:
    reader_screen_deactivate((struct Event){0}, *out);
    break;

  default:;
  }

  event_bus_unregister((*out)->bus, BusConnectorEnum_READER_SCREEN,
                       post_reader_screen_event, *out);

  mem_free(*out);
  *out = NULL;
};

static void post_reader_screen_event(struct Event event, void *data) {
  puts(__func__);
  struct ReaderScreenTransition action;
  reader_screen_t rscreen = data;

  action = fsm_table[rscreen->current_state][event.event];
  if (!action.action) {
    return;
  }

  if (rscreen->current_state != action.next_state) {
    log_info("%s -> %s", reader_screen_state_dump(rscreen->current_state),
             reader_screen_state_dump(action.next_state));
  }

  action.action(event, data);
  rscreen->current_state = action.next_state;
}

static void reader_screen_activate(struct Event event, void *data) {
  puts(__func__);
  const unsigned char *page_data;
  reader_screen_t rscreen = data;
  book_t book = event.data;
  int page_size = 0;

  page_data =
      book_get_page(book, lv_display_get_horizontal_resolution(NULL),
                    lv_display_get_vertical_resolution(NULL), &page_size);
  if (!page_data) {
    ERR_TRY(err_o);
  }

  assert(page_size != 0);

  wx_reader_t reader = wx_reader_create(page_size, page_data);
  if (!reader) {
    err_o = err_errnos(EINVAL, "Cannot create reader widget");
    goto error_out;
  }

  lv_obj_set_user_data(reader, mem_ref(book));

  display_add_to_ingroup(rscreen->display, reader);
  lv_obj_add_event_cb(reader, reader_screen_event_cb, LV_EVENT_KEY, rscreen);

  rscreen->ctx = (struct ReaderScreenCtx){
      .reader = reader,
      .scale = book_get_scale(book),
      .x_off = book_get_x_off(book),
      .y_off = book_get_y_off(book),
      .page = book_get_page_no(book),
      .book = book,
  };

  return;

error_out:;
  reader_screen_deactivate(event, data);
  // @todo: post error
}

static void reader_screen_deactivate(struct Event event, void *data) {
  puts(__func__);
  reader_screen_t rscreen = data;
  if (rscreen->ctx.reader) {
    book_t book = lv_obj_get_user_data(rscreen->ctx.reader);
    reader_screen_put_on_hold(event, data);
    mem_deref(book);
    wx_reader_destroy(rscreen->ctx.reader);
    memset(&rscreen->ctx, 0, sizeof(struct ReaderScreenCtx));
  }
}

static const char *reader_screen_state_dump(enum ReaderScreenState state) {
  static char *dumps[] = {
      [ReaderScreenState_NONE] = "reader_screen_none",
      [ReaderScreenState_ACTIVE] = "reader_screen_activated",
  };

  if (state < ReaderScreenState_NONE || state >= ReaderScreenState_MAX ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void reader_screen_event_cb(lv_event_t *e) {
  /* wx_reader_book_t wx = lv_event_get_current_target(e); */
  reader_screen_t rscreen = lv_event_get_user_data(e);
  lv_key_t key = lv_event_get_key(e);
  /* book_t book = wx_reader_book_get_data(wx); */

  if (key == '\r' || key == '\n' || key == LV_KEY_ENTER) {
    event_bus_post_event(
        rscreen->bus, BusEnum_READER_SCREEN,
        (struct Event){.event = EventEnum_BTN_ENTER, .data = NULL});
  } else if (key == LV_KEY_LEFT) {
    event_bus_post_event(
        rscreen->bus, BusEnum_READER_SCREEN,
        (struct Event){.event = EventEnum_BTN_LEFT, .data = NULL});
  } else if (key == LV_KEY_RIGHT) {
    event_bus_post_event(
        rscreen->bus, BusEnum_READER_SCREEN,
        (struct Event){.event = EventEnum_BTN_RIGHT, .data = NULL});
  } else if (key == LV_KEY_UP) {
    event_bus_post_event(
        rscreen->bus, BusEnum_READER_SCREEN,
        (struct Event){.event = EventEnum_BTN_UP, .data = NULL});
  } else if (key == LV_KEY_DOWN) {
    event_bus_post_event(
        rscreen->bus, BusEnum_READER_SCREEN,
        (struct Event){.event = EventEnum_BTN_DOWN, .data = NULL});
  } else if (key == LV_KEY_ESC) {
    event_bus_post_event(
        rscreen->bus, BusEnum_ALL,
        (struct Event){.event = EventEnum_BTN_MENU, .data = NULL});
  }
}

static void reader_screen_refresh(struct Event event, void *data) {
  puts(__func__);
  reader_screen_t rscreen = data;
  book_t book = lv_obj_get_user_data(rscreen->ctx.reader);

  mem_ref(book);

  struct ReaderScreenCtx new_ctx = {.reader = rscreen->ctx.reader,
                                    .scale = book_get_scale(book),
                                    .x_off = book_get_x_off(book),
                                    .y_off = book_get_y_off(book),
                                    .page = book_get_page_no(book),
                                    .book = book};
  if (memcmp(&rscreen->ctx, &new_ctx, sizeof(struct ReaderScreenCtx)) != 0) {
    reader_screen_deactivate(event, data);
    reader_screen_activate(event, data);
  }

  mem_deref(book);
}

static void reader_screen_put_on_hold(struct Event event, void *data) {
  reader_screen_t rscreen = data;
  display_del_from_ingroup(rscreen->display, rscreen->ctx.reader);
  lv_obj_remove_event_cb_with_user_data(rscreen->ctx.reader,
                                        reader_screen_event_cb, rscreen);
}

static void reader_screen_resume(struct Event event, void *data) {
  reader_screen_t rscreen = data;
  display_add_to_ingroup(rscreen->display, rscreen->ctx.reader);
  lv_obj_add_event_cb(rscreen->ctx.reader, reader_screen_event_cb, LV_EVENT_KEY,
                      rscreen);
}
