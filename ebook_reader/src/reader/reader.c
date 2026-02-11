#include <assert.h>
#include <stdio.h>

#include "event_bus/event_bus.h"
#include "library/library.h"
#include "reader/reader.h"
#include "utils/log.h"
#include "utils/mem.h"

enum ReaderState {
  ReaderState_NONE,
  ReaderState_ACTIVE,
  ReaderState_MAX,
};

struct Reader {
  enum ReaderState current_state;
  book_t book;
  bus_t bus;
};

struct ReaderTransition {
  enum ReaderState next_state;
  post_event_t action;
};

static void post_reader_event(struct Event event, void *data);
static void reader_activate(struct Event event, void *data);
static void reader_deactivate(struct Event event, void *data);
static void reader_prev_page(struct Event event, void *data);
static void reader_next_page(struct Event event, void *data);
static void reader_put_on_hold(struct Event event, void *data);
static void reader_resume(struct Event event, void *data);
static const char *reader_state_dump(enum ReaderState state);

static struct ReaderTransition fsm_table[ReaderState_MAX][EventEnum_MAX] = {
    [ReaderState_NONE] =
        {
            [EventEnum_BOOK_OPENED] =
                {
                    .action = reader_activate,
                    .next_state = ReaderState_ACTIVE,
                },
        },
    [ReaderState_ACTIVE] =
        {
            [EventEnum_BTN_LEFT] =
                {
                    .action = reader_prev_page,
                    .next_state = ReaderState_ACTIVE,
                },
            [EventEnum_BTN_RIGHT] =
                {
                    .action = reader_next_page,
                    .next_state = ReaderState_ACTIVE,
                },
            [EventEnum_BTN_MENU] =
                {
                    .action = reader_deactivate,
                    .next_state = ReaderState_NONE,
                },
            [EventEnum_BOOK_SETTINGS_ACTIVATED] =
                {
                    .action = reader_put_on_hold,
                    .next_state = ReaderState_ACTIVE,
                },
            [EventEnum_BOOK_SETTINGS_DEACTIVATED] =
                {
                    .action = reader_resume,
                    .next_state = ReaderState_ACTIVE,
                },
        },
};

err_t reader_init(reader_t *out, bus_t bus) {
  reader_t reader = *out = mem_malloc(sizeof(struct Reader));
  *reader = (struct Reader){
      .bus = bus,
  };

  event_bus_register(bus, BusConnectorEnum_READER, post_reader_event, reader);

  return 0;
};

void reader_destroy(reader_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  mem_deref((*out)->book);

  event_bus_unregister((*out)->bus, BusConnectorEnum_READER, post_reader_event,
                       *out);

  mem_free(*out);
  *out = NULL;
};

static void post_reader_event(struct Event event, void *data) {
  puts(__func__);
  struct ReaderTransition action;
  reader_t reader = data;

  action = fsm_table[reader->current_state][event.event];
  if (!action.action) {
    return;
  }

  if (reader->current_state != action.next_state) {
    log_info("%s -> %s", reader_state_dump(reader->current_state),
             reader_state_dump(action.next_state));
  }

  action.action(event, data);
  reader->current_state = action.next_state;
}

static const char *reader_state_dump(enum ReaderState state) {
  static char *dumps[] = {
      [ReaderState_NONE] = "reader_none",
      [ReaderState_ACTIVE] = "reader_activated",
  };

  if (state < ReaderState_NONE || state >= ReaderState_MAX || !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void reader_activate(struct Event event, void *data) {
  reader_t reader = data;
  reader->book = mem_ref(event.data);
};

static void reader_deactivate(struct Event event, void *data) {
  reader_t reader = data;
  mem_deref(reader->book);
  reader->book = NULL;
};

static void reader_prev_page(struct Event event, void *data) {
  reader_t reader = data;

  int page_no = book_get_page_no(reader->book);
  page_no--;
  book_set_page_no(reader->book, page_no);

  event_bus_post_event(reader->bus, BusEnum_READER,
                       (struct Event){
                           .event = EventEnum_BOOK_UPDATED,
                           .data = reader->book,
                       });
}

static void reader_next_page(struct Event event, void *data) {
  reader_t reader = data;

  int page_no = book_get_page_no(reader->book);
  page_no++;
  book_set_page_no(reader->book, page_no);

  event_bus_post_event(reader->bus, BusEnum_READER,
                       (struct Event){
                           .event = EventEnum_BOOK_UPDATED,
                           .data = reader->book,
                       });
}

static void reader_put_on_hold(struct Event event, void *data) {}
static void reader_resume(struct Event event, void *data) {}
