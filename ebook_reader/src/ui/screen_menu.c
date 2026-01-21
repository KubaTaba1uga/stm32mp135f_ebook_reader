#include "core/lv_group.h"
#include "ui/screen.h"
#include "ui/widgets.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"
#include <stdio.h>

typedef struct UiScreenMenu *ui_screen_menu_t;

struct UiScreenMenu {
  ui_wx_bar_t bar;
  ui_wx_menu_t menu;
  struct {
    ui_wx_menu_book_t *buf;
    int len;
  } books;
  lv_group_t *group;
  ui_t owner;
};

static void ui_screen_menu_destroy(void *);

err_t ui_screen_menu_create(ui_screen_t *out, ui_t ui, books_list_t books,
                            int book_i, int event,
                            void (*event_cb)(lv_event_t *e),
                            lv_group_t *group) {
  ui_wx_bar_t bar = ui_wx_bar_create();
  if (!bar) {
    err_o = err_errnos(EINVAL, "Cannot create bar widget");
    goto error_out;
  }

  ui_wx_menu_t menu = ui_wx_menu_create();
  if (!menu) {
    err_o = err_errnos(EINVAL, "Cannot create menu widget");
    goto error_bar_cleanup;
  }

  lv_obj_t **lv_books = mem_malloc(sizeof(lv_obj_t *) * books_list_len(books));
  int lv_books_len = books_list_len(books);
  lv_obj_t *lv_book = NULL;
  int i = 0;

  lv_group_add_obj(group, menu);

  for (book_t book = books_list_get(books); book != NULL;
       book = books_list_get(books)) {
    lv_book = ui_wx_menu_book_create(
        menu, book_get_title(book), lv_book == NULL,
        book_get_thumbnail(book, menu_book_x, menu_book_y - menu_book_text_y),
        i, ui);
    lv_obj_add_event_cb(lv_book, event_cb, event, lv_book);
    lv_books[i++] = lv_book;
  }

  ui_screen_menu_t screen = mem_malloc(sizeof(struct UiScreenMenu));
  *screen = (struct UiScreenMenu){
      .bar = bar,
      .menu = menu,
      .books =
          {
              .buf = lv_books,
              .len = lv_books_len,
          },
      .group = group,
      .owner = ui,
  };

  err_o = ui_screen_create(out, ui, ui_screen_menu_destroy, screen);
  ERR_TRY_CATCH(err_o, error_screen_cleanup);

  return 0;

error_screen_cleanup:
  ui_screen_menu_destroy(screen);
error_bar_cleanup:
  ui_wx_bar_destroy(bar);
error_out:
  return err_o;
};

static void ui_screen_menu_destroy(void *screen) {
  ui_screen_menu_t menu = screen;
  for (int i = menu->books.len - 1; i >= 0; i--) {
    ui_wx_menu_book_destroy(menu->books.buf[i]);
  }

  mem_free(menu->books.buf);
  ui_wx_menu_destroy(menu->menu);
  ui_wx_bar_destroy(menu->bar);
  mem_free(menu);
}
