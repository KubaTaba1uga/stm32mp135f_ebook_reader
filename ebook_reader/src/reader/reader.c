#include <stdio.h>

#include "display/display.h"
#include "event_queue/event_queue.h"
#include "library/library.h"
#include "reader/core.h"
#include "reader/reader.h"
#include "utils/log.h"
#include "utils/mem.h"

enum ReaderStates {
  ReaderStates_NONE,
  ReaderStates_ACTIVE,
  ReaderStates_BACKGROUND,
  ReaderStates_MAX,
};

struct Reader {
  enum ReaderStates current_state;
  event_queue_t evqueue;
  display_t display;
  library_t library;
  struct ReaderView view;
};

struct ReaderTransition {
  enum ReaderStates next_state;
  post_event_func_t action;
};

static void reader_activate(enum Events __, ref_t book, void *sub_data);
static void reader_deactivate(enum Events __, ref_t ___, void *sub_data);
static void reader_next_page(enum Events __, ref_t ___, void *sub_data);
static void reader_prev_page(enum Events __, ref_t ___, void *sub_data);
static void reader_put_in_bg(enum Events __, ref_t ___, void *sub_data);
static void reader_refresh(enum Events __, ref_t ___, void *sub_data);
static void reader_post_event(enum Events event, ref_t event_data,
                              void *sub_data);
static const char *reader_state_dump(enum ReaderStates state);
static void next_page_cb(void *);
static void prev_page_cb(void *);
static void menu_cb(void *);
static void book_settings_cb(void *);

struct ReaderTransition reader_fsm_table[ReaderStates_MAX][Events_MAX] = {
    [ReaderStates_NONE] =
        {
            [Events_BOOK_OPENED] =
                {
                    .next_state = ReaderStates_ACTIVE,
                    .action = reader_activate,
                },
        },
    [ReaderStates_ACTIVE] =
        {
            [Events_BTN_NEXT_PAGE_CLICKED] =
                {
                    .next_state = ReaderStates_ACTIVE,
                    .action = reader_next_page,
                },
            [Events_BTN_PREV_PAGE_CLICKED] =
                {
                    .next_state = ReaderStates_ACTIVE,
                    .action = reader_prev_page,
                },
            [Events_BOOK_UPDATED] =
                {
                    .next_state = ReaderStates_ACTIVE,
                    .action = reader_refresh,
                },
            [Events_BOOK_CLOSED] =
                {
                    .next_state = ReaderStates_NONE,
                    .action = reader_deactivate,
                },
            [Events_BTN_MENU_CLICKED] =
                {
                    .next_state = ReaderStates_NONE,
                    .action = reader_deactivate,
                },
            [Events_BOOK_SETTINGS_OPENED] =
                {
                    .next_state = ReaderStates_BACKGROUND,
                    .action = reader_put_in_bg,
                },
        },

};

err_t reader_init(reader_t *out, display_t display, event_queue_t evqueue,
                  library_t library) {
  puts(__func__);
  reader_t reader = *out = mem_malloc(sizeof(struct Reader));
  *reader = (struct Reader){
      .current_state = ReaderStates_NONE,
      .evqueue = evqueue,
      .display = display,
      .library = library,
  };

  event_queue_register(evqueue, EventSubscribers_READER, reader_post_event,
                       reader);

  return 0;
};

void reader_destroy(reader_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  reader_t reader = *out;

  switch (reader->current_state) {
  case ReaderStates_ACTIVE:
    reader_deactivate(Events_NONE, NULL, reader);
    break;
  default:;
  }

  event_queue_deregister(reader->evqueue, EventSubscribers_READER);
  mem_free(*out);
  *out = NULL;
}

static void reader_post_event(enum Events event, ref_t event_data,
                              void *sub_data) {
  puts(__func__);
  struct ReaderTransition action;
  reader_t reader = sub_data;

  action = reader_fsm_table[reader->current_state][event];
  if (!action.action) {
    return;
  }

  if (reader->current_state != action.next_state) {
    log_info("%s -> %s", reader_state_dump(reader->current_state),
             reader_state_dump(action.next_state));
  }

  action.action(event, event_data, sub_data);
  reader->current_state = action.next_state;
}

static const char *reader_state_dump(enum ReaderStates state) {
  static char *dumps[ReaderStates_MAX] = {
      [ReaderStates_NONE] = "reader_none",
      [ReaderStates_ACTIVE] = "reader_activated",
      [ReaderStates_BACKGROUND] = "reader_background",
  };

  if (state < ReaderStates_NONE || state >= ReaderStates_MAX || !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void reader_activate(enum Events __, ref_t arg, void *sub_data) {
  puts(__func__);
  reader_t reader = sub_data;
  book_t book = arg;

  err_o = reader_view_init(&reader->view, book, next_page_cb, prev_page_cb,
                           menu_cb, book_settings_cb, reader);
  ERR_TRY(err_o);

  display_add_to_ingroup(reader->display, reader->view.page);

  return;

error_out:;
  // @todo post error event
};

static void reader_deactivate(enum Events __, ref_t ___, void *sub_data) {
  puts(__func__);
  reader_t reader = sub_data;

  display_del_from_ingroup(reader->display, reader->view.page);
  reader_view_destroy(&reader->view);
};

static void next_page_cb(void *out) {
  reader_t reader = out;

  event_queue_push(reader->evqueue, Events_BTN_NEXT_PAGE_CLICKED, NULL);
}

static void prev_page_cb(void *out) {
  reader_t reader = out;
  event_queue_push(reader->evqueue, Events_BTN_PREV_PAGE_CLICKED, NULL);
}

static void menu_cb(void *out) {
  reader_t reader = out;

  event_queue_push(reader->evqueue, Events_BTN_MENU_CLICKED, NULL);
}

static void book_settings_cb(void *out) {
  reader_t reader = out;

  event_queue_push(reader->evqueue, Events_BOOK_SETTINGS_OPENED,
                   reader->view.book);
}

static void reader_next_page(enum Events __, ref_t ___, void *sub_data) {
  reader_t reader = sub_data;

  int page_no = book_get_page_no(reader->view.book);
  page_no++;
  book_set_page_no(reader->view.book, page_no);

  event_queue_push(reader->evqueue, Events_BOOK_UPDATED, reader->view.book);
}

static void reader_prev_page(enum Events __, ref_t ___, void *sub_data) {
  reader_t reader = sub_data;

  int page_no = book_get_page_no(reader->view.book);
  page_no--;
  book_set_page_no(reader->view.book, page_no);

  event_queue_push(reader->evqueue, Events_BOOK_UPDATED, reader->view.book);
}

static void reader_refresh(enum Events __, ref_t ___, void *sub_data) {
  reader_t reader = sub_data;
  display_del_from_ingroup(reader->display, reader->view.page);
  reader_view_refresh(&reader->view);
  display_add_to_ingroup(reader->display, reader->view.page);
}

#include "lvgl.h"
static void reader_put_in_bg(enum Events __, ref_t ___, void *sub_data) {
  reader_t reader = sub_data;

  display_del_from_ingroup(reader->display, reader->view.page);

  uint32_t n = lv_group_get_obj_count(lv_group_get_default());
  log_info("group=%p count=%u", (void *)lv_group_get_default(), (unsigned)n);
  
}
