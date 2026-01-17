#include "ui/screen.h"
#include "ui/widgets.h"
#include "utils/mem.h"

typedef struct UiScreenMenu *ui_screen_menu_t;

struct UiScreenMenu {
  ui_wx_menu_t menu;
  struct {
    ui_wx_menu_book_t *buf;
    int len;
  } books;
  lv_group_t *group;
  ui_t owner;
};

static void ui_screen_menu_destroy(void *);

err_t ui_screen_menu_create(ui_screen_t *screen, ui_t ui, books_list_t books,
                            int book_i, int event,
                            void (*event_cb)(lv_event_t *e)) {
  puts(__func__);
  ui_screen_menu_t menu = mem_malloc(sizeof(struct UiScreenMenu));
  *menu = (struct UiScreenMenu){
      .owner = ui,
  };

  err_o = ui_screen_init(screen, ui, ui_screen_menu_destroy, menu);
  ERR_TRY(err_o);

  menu->menu = ui_wx_menu_create();
  if (!menu->menu) {
    err_o = err_errnos(EINVAL, "Cannot create menu widget");
    goto error_screen_cleanup;
  }

  menu->books.buf = mem_malloc(sizeof(lv_obj_t *) * books_list_len(books));
  menu->books.len = books_list_len(books);
  lv_obj_t **lv_books = menu->books.buf;
  lv_obj_t *lv_book = NULL;
  int i = 0;

  lv_group_t *group = menu->group = lv_group_create();
  lv_group_set_default(group);
  for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i)) {
    if (lv_indev_get_type(i) == LV_INDEV_TYPE_KEYPAD) {
      lv_indev_set_group(i, group);
      break;
    }
  }
  lv_group_add_obj(group, menu->menu);
  lv_group_set_editing(group, false);

  for (book_t book = books_list_get(books); book != NULL;
       book = books_list_get(books)) {
    lv_book = ui_wx_menu_book_create(
        menu->menu, book_get_title(book), lv_book == NULL,
        book_get_thumbnail(book, menu_book_x, menu_book_y - menu_book_text_y),
        i, ui);
    lv_obj_add_event_cb(lv_book, event_cb, event, lv_book);
    lv_books[i++] = lv_book;
  }

  return 0;

error_screen_cleanup:
  ui_screen_destroy(screen);
  goto error;
error_out:
  mem_free(menu);
error:
  return err_o;
};

static void ui_screen_menu_destroy(void *screen) {
  ui_screen_menu_t menu = screen;
  for (int i = menu->books.len - 1; i >= 0; i--) {
    ui_wx_menu_book_destroy(menu->books.buf[i]);
  }

  mem_free(menu->books.buf);
  ui_wx_menu_destroy(menu->menu);
  lv_group_del(menu->group);
  
  mem_free(menu);
}
