#include <error.h>
#include <stdio.h>
#include <string.h>

#include "ui/display.h"
#include "ui/screen.h"
#include "ui/screen_menu.h"
#include "ui/ui.h"
#include "ui/widgets.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/time.h"

struct Ui {
  struct {
    void (*callback)(enum UiInputEventEnum event, void *data, void *arg);
    void *data;
  } inputh;

  ui_display_t display;
  ui_screen_t screen;
  ui_wx_bar_t bar;
};

static void ui_menu_book_event_cb(lv_event_t *e);

err_t ui_create(ui_t *out,
              void (*callback)(enum UiInputEventEnum event, void *data,
                               void *arg),
              void *data) {
  ui_t ui = *out = mem_malloc(sizeof(struct Ui));
  *ui = (struct Ui){
      .inputh =
          {
              .callback = callback,
              .data = data,
          },
  };

  lv_init();
  lv_tick_set_cb(time_now);

  err_o = ui_display_supported_create(&ui->display, ui, settings_display_model);
  ERR_TRY(err_o);

  err_o = ui_display_show_boot_img(ui->display, settings_boot_screen_path);
  ERR_TRY_CATCH(err_o, error_display_cleanup);

  return 0;

error_display_cleanup:
  ui_display_destroy(&ui->display);
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
  ui_screen_destroy(&ui->screen);    
  ui_display_destroy(&ui->display);
  mem_free(*out);
  *out = NULL;
};

err_t ui_menu_create(ui_t ui, books_list_t books, int book_i) {
  err_o = ui_screen_menu_create(&ui->screen, ui, books, book_i, LV_EVENT_KEY,
                                ui_menu_book_event_cb);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

void ui_menu_destroy(ui_t ui) { ui_screen_destroy(&ui->screen); };

void ui_panic(ui_t ui) {
  puts(__func__);
  ui_display_panic(ui->display);
};

void ui_render_cleanup(ui_t ui) { ui_display_render_destroy(ui->display); };

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
