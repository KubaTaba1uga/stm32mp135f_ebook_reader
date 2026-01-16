#include "ui/core.h"
#include <error.h>
#include <stdio.h>

#include "misc/lv_timer.h"
#include "ui/display.h"
#include "ui/ui.h"
#include "ui/widgets.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/time.h"

struct Ui {
  struct {
    void (*callback)(enum UiInputEventEnum event, void *data, void *arg);
    void *data;
  } inputh;

  enum UiDisplayEnum current_display;
  struct UiDisplay display;
  struct UiMenu menu;
  ui_wx_bar_t bar;
};

static void ui_menu_book_event_cb(lv_event_t *e);

err_t ui_init(ui_t *out,
              void (*callback)(enum UiInputEventEnum event, void *data,
                               void *arg),
              void *data) {
  static err_t (*displays_inits[])(ui_display_t, ui_t) = {
      [UiDisplayEnum_X11] = ui_display_x11_init,
  };
  ui_t ui = *out = mem_malloc(sizeof(struct Ui));
  *ui = (struct Ui){
      .inputh = {.callback = callback, .data = data},
  };

  lv_init();
  lv_tick_set_cb(time_now);

  /**
     @todo This should be in display_init
  */
  bool is_display_found = false;
  for (int i = UiDisplayEnum_X11; i < sizeof(displays_inits) / sizeof(void *);
       i++) {
    if (!displays_inits[i]) {
      continue;
    }

    err_o = displays_inits[i](&ui->display, ui);
    if (!err_o) {
      ui->current_display = i;
      is_display_found = true;
      break;
    }

    log_error(err_o);
  }

  if (!is_display_found) {
    err_o = err_errnos(ENODEV, "Cannot initialize display");
    goto error_out;
  }

  /**
     @todo This should be in display render boot img.
  */
  FILE *boot_screen_fd = fopen(settings_boot_screen_path, "r");
  if (!boot_screen_fd) {
    err_o = err_errnof(ENOENT, "There is no file like %s",
                       settings_boot_screen_path);
    goto error_display_cleanup;
  }

  unsigned char *img_buf = mem_malloc(48000);
  const size_t ret_code = fread(img_buf, 1, 48000, boot_screen_fd);
  if (ret_code != 48000) {
    err_o =
        err_errnof(ENOENT, "Cannot read file %s", settings_boot_screen_path);
    goto error_boot_screen_cleanup;
  }
  fclose(boot_screen_fd);

  err_o = ui->display.render(&ui->display, (unsigned char *)img_buf);
  ERR_TRY_CATCH(err_o, error_boot_screen_cleanup);

  return 0;

error_boot_screen_cleanup:
  fclose(boot_screen_fd);
  mem_free(img_buf);
error_display_cleanup:
  ui->display.destroy(&ui->display);
error_out:
  mem_free(*out);
  *out = NULL;
  return err_o;
}

int ui_tick(ui_t ui) { return lv_timer_handler(); };

void ui_destroy(ui_t *out) {
  if (!out || !*out) {
    return;
  }

  ui_t ui = *out;
  ui->display.destroy(&ui->display);
  mem_free(*out);
  *out = NULL;
};

err_t ui_menu_create(ui_t ui, books_list_t books, int book_i) {
  ui->bar = ui->bar ? ui->bar : ui_wx_bar_create();
  ui->menu.menu = ui_wx_menu_create();
  ui->menu.books = mem_malloc(sizeof(lv_obj_t *) * books_list_len(books));

  if (!ui->bar || !ui->menu.menu) {
    err_o = err_errnos(EINVAL, "`ui->bar` && `ui->menu.menu` cannot be NULL");
    goto error_out;
  }
  lv_obj_t **lv_books = ui->menu.books;
  lv_obj_t *lv_book = NULL;
  int i = 0;

  { // Configure passing events to menu widget
    lv_group_t *group = ui->menu.group = lv_group_create();
    lv_group_set_default(group);

    for (lv_indev_t *i = lv_indev_get_next(NULL); i; i = lv_indev_get_next(i)) {
      if (lv_indev_get_type(i) == LV_INDEV_TYPE_KEYPAD) {
        lv_indev_set_group(i, group);
        break;
      }
    }

    lv_group_add_obj(group, ui->menu.menu);
    lv_group_set_editing(group, false);
  }

  for (book_t book = books_list_get(books); book != NULL;
       book = books_list_get(books)) {
    lv_book = ui_wx_menu_book_create(
        ui->menu.menu, book_get_title(book), lv_book == NULL,
        book_get_thumbnail(book, menu_book_x, menu_book_y - menu_book_text_y),
        i, ui);

    lv_obj_add_event_cb(lv_book, ui_menu_book_event_cb, LV_EVENT_KEY, lv_book);

    lv_books[i++] = lv_book;
  }

  return 0;

error_out:
  return err_o;
};

void ui_menu_destroy(ui_t ui) { return; };

static void ui_menu_book_event_cb(lv_event_t *e) {
  ui_wx_menu_book_t book = lv_event_get_user_data(e);
  ui_t ui = ui_wx_menu_book_get_ui(book);
  int id = ui_wx_menu_book_get_id(book);

  if (lv_event_get_code(e) == LV_EVENT_KEY) {
    lv_key_t key = lv_event_get_key(e);
    if (key == '\r' || key == '\n') {
      key = LV_KEY_ENTER;
    }

    if (key == LV_KEY_ENTER) {
      ui->inputh.callback(UiInputEventEnum_ENTER, ui->inputh.data, &id);
    }
  }
}

void ui_panic(ui_t ui) {
  puts(__func__);
  if (!ui || !ui->display.panic) {
    return;
  }

  ui->display.panic(&ui->display);
};
