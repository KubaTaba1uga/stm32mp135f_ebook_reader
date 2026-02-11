#include <assert.h>
#include <stdio.h>

#include "book/book.h"
#include "display/display.h"
#include "event_bus/event_bus.h"
#include "library/library.h"
#include "menu_screen/core.h"
#include "menu_screen/menu_screen.h"
#include "misc/lv_event.h"
#include "utils/log.h"
#include "utils/mem.h"

enum MenuScreenState {
  MenuScreenState_NONE,
  MenuScreenState_ACTIVE,
  MenuScreenState_MAX,
};

struct MenuScreen {
  enum MenuScreenState current_state;
  books_list_t books;
  display_t display;
  bus_t bus;
  struct {
    wx_bar_t bar;
    wx_menu_t menu;
    wx_menu_book_t *books;
    int books_len;
  } ctx;
};

struct MenuScreenTransition {
  enum MenuScreenState next_state;
  post_event_t action;
};

static void post_menu_screen_event(struct Event event, void *data);
static void menu_screen_activate(struct Event event, void *data);
static void menu_screen_deactivate(struct Event event, void *data);
static const char *menu_screen_state_dump(enum MenuScreenState state);
static void menu_screen_event_cb(lv_event_t *e);

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

err_t menu_screen_init(menu_screen_t *out, display_t display, bus_t bus) {
  menu_screen_t mscreen = *out = mem_malloc(sizeof(struct MenuScreen));
  *mscreen = (struct MenuScreen){
      .display = display,
      .bus = bus,
  };

  event_bus_register(bus, BusConnectorEnum_MENU_SCREEN, post_menu_screen_event,
                     mscreen);

  return 0;
};

void menu_screen_destroy(menu_screen_t *out) {
  if (!out || !*out) {
    return;
  }

  switch ((*out)->current_state) {
  case MenuScreenState_ACTIVE:
    menu_screen_deactivate((struct Event){0}, *out);
    break;

  default:;
  }

  event_bus_unregister((*out)->bus, BusConnectorEnum_MENU_SCREEN,
                       post_menu_screen_event, *out);

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
  books_list_t books = event.data;
  menu_screen_t mscreen = data;

  wx_bar_t bar = wx_bar_create();
  if (!bar) {
    err_o = err_errnos(EINVAL, "Cannot create bar widget");
    goto error_out;
  }

  wx_menu_t menu = wx_menu_create();
  if (!menu) {
    err_o = err_errnos(EINVAL, "Cannot create menu widget");
    goto error_bar_cleanup;
  }

  if (!books) {
    goto out;
  }

  lv_obj_t **lv_books = mem_malloc(sizeof(lv_obj_t *) * books_list_len(books));
  lv_obj_t *lv_book = NULL;
  int i = 0;

  for (book_t book = books_list_get(books); book != NULL;
       book = books_list_get(books)) {
    lv_book = wx_menu_book_create(
        menu, book_get_title(book), lv_book == NULL,
        book_get_thumbnail(book, menu_book_x, menu_book_y - menu_book_text_y),
        mem_ref(book));
    lv_obj_add_event_cb(lv_book, menu_screen_event_cb, LV_EVENT_KEY, mscreen);

    lv_books[i++] = lv_book;
  }

  mscreen->ctx.bar = bar;
  mscreen->ctx.menu = menu;
  mscreen->ctx.books = lv_books;
  mscreen->ctx.books_len = books_list_len(books);
  /* mscreen->books = mem_ref(books); */

out:
  display_add_to_ingroup(mscreen->display, menu);
  return;

error_bar_cleanup:
  wx_bar_destroy(bar);
error_out:;
  menu_screen_deactivate(event, data);
  // @todo: post error
}
static void menu_screen_deactivate(struct Event event, void *data) {
  puts(__func__);
  menu_screen_t mscreen = data;

  if (mscreen->ctx.books) {
    for (int i = mscreen->ctx.books_len - 1; i >= 0; i--) {
      if (mscreen->ctx.books[i]) {
        book_t book = wx_menu_book_get_data(mscreen->ctx.books[i]);
        mem_deref(book);
        wx_menu_book_destroy(mscreen->ctx.books[i]);
      }
    }
    mem_free(mscreen->ctx.books);
    mscreen->ctx.books = NULL;
  }

  if (mscreen->ctx.menu) {
    wx_menu_destroy(mscreen->ctx.menu);
    mscreen->ctx.menu = NULL;
  }

  if (mscreen->ctx.bar) {
    wx_bar_destroy(mscreen->ctx.bar);
    mscreen->ctx.bar = NULL;
  }
}

static const char *menu_screen_state_dump(enum MenuScreenState state) {
  static char *dumps[] = {
      [MenuScreenState_NONE] = "menu_screen_none",
      [MenuScreenState_ACTIVE] = "menu_screen_activated",
  };

  if (state < MenuScreenState_NONE || state >= MenuScreenState_MAX ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void menu_screen_event_cb(lv_event_t *e) {
  wx_menu_book_t wx = lv_event_get_current_target(e);
  menu_screen_t mscreen = lv_event_get_user_data(e);
  lv_key_t key = lv_event_get_key(e);
  book_t book = wx_menu_book_get_data(wx);

  if (key == '\r' || key == '\n' || key == LV_KEY_ENTER) {
    event_bus_post_event(
        mscreen->bus, BusEnum_MENU_SCREEN,
        (struct Event){.event = EventEnum_BTN_ENTER, .data = book});
  }
}
