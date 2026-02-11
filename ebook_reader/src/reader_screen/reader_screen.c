#include <assert.h>
#include <lvgl.h>
#include <stdio.h>

#include "display/display.h"
#include "event_bus/event_bus.h"
#include "library/library.h"
#include "reader_screen/reader_screen.h"
#include "utils/log.h"
#include "utils/mem.h"

enum ReaderScreenState {
  ReaderScreenState_NONE,
  ReaderScreenState_ACTIVE,
  ReaderScreenState_MAX,
};

struct ReaderScreen {
  enum ReaderScreenState current_state;
  display_t display;
  book_t book;
  bus_t bus;
  struct {
  } ctx;
};

struct ReaderScreenTransition {
  enum ReaderScreenState next_state;
  post_event_t action;
};

static void post_reader_screen_event(struct Event event, void *data);
static void reader_screen_activate(struct Event event, void *data);
static void reader_screen_deactivate(struct Event event, void *data);
static const char *reader_screen_state_dump(enum ReaderScreenState state);
static void reader_screen_event_cb(lv_event_t *e);

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
        [ReaderScreenState_ACTIVE] =
            {
                [EventEnum_BOOK_CLOSED] =
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
  
  event_bus_register(bus, BusConnectorEnum_READER_SCREEN, post_reader_screen_event,
                     rscreen);

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
  (void)reader_screen_event_cb;
  /* books_list_t books = event.data; */
  /* reader_screen_t rscreen = data; */

  /* wx_bar_t bar = wx_bar_create(); */
  /* if (!bar) { */
  /*   err_o = err_errnos(EINVAL, "Cannot create bar widget"); */
  /*   goto error_out; */
  /* } */

  /* wx_reader_t reader = wx_reader_create(); */
  /* if (!reader) { */
  /*   err_o = err_errnos(EINVAL, "Cannot create reader widget"); */
  /*   goto error_bar_cleanup; */
  /* } */

  /* if (!books) { */
  /*   goto out; */
  /* } */

  /* lv_obj_t **lv_books = mem_malloc(sizeof(lv_obj_t *) * books_list_len(books)); */
  /* lv_obj_t *lv_book = NULL; */
  /* int i = 0; */

  /* for (book_t book = books_list_get(books); book != NULL; */
  /*      book = books_list_get(books)) { */
  /*   lv_book = wx_reader_book_create( */
  /*       reader, book_get_title(book), lv_book == NULL, */
  /*       book_get_thumbnail(book, reader_book_x, reader_book_y - reader_book_text_y), */
  /*       mem_ref(book)); */
  /*   lv_obj_add_event_cb(lv_book, reader_screen_event_cb, LV_EVENT_KEY, rscreen); */

  /*   lv_books[i++] = lv_book; */
  /* } */

  /* rscreen->ctx.bar = bar; */
  /* rscreen->ctx.reader = reader; */
  /* rscreen->ctx.books = lv_books; */
  /* rscreen->ctx.books_len = books_list_len(books); */
  /* rscreen->books = mem_ref(books); */

/* out: */
/*   display_add_to_ingroup(rscreen->display, reader); */
/*   return; */

/* error_bar_cleanup: */
/*   wx_bar_destroy(bar); */
/* error_out:; */
/*   reader_screen_deactivate(event, data); */
  // @todo: post error
}
static void reader_screen_deactivate(struct Event event, void *data) {
  puts(__func__);
  /* reader_screen_t rscreen = data; */

  /* if (rscreen->ctx.books) { */
  /*   for (int i = rscreen->ctx.books_len - 1; i >= 0; i--) { */
  /*     if (rscreen->ctx.books[i]) { */
  /*       book_t book = wx_reader_book_get_data(rscreen->ctx.books[i]); */
  /*       mem_deref(book); */
  /*       wx_reader_book_destroy(rscreen->ctx.books[i]); */
  /*     } */
  /*   } */
  /*   mem_free(rscreen->ctx.books); */
  /*   rscreen->ctx.books = NULL; */
  /* } */

  /* if (rscreen->ctx.reader) { */
  /*   wx_reader_destroy(rscreen->ctx.reader); */
  /*   rscreen->ctx.reader = NULL; */
  /* } */

  /* if (rscreen->ctx.bar) { */
  /*   wx_bar_destroy(rscreen->ctx.bar); */
  /*   rscreen->ctx.bar = NULL; */
  /* } */
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
        rscreen->bus, BusEnum_READER_SCREEN,
        (struct Event){.event = EventEnum_BTN_MENU, .data = NULL});
  }
}
