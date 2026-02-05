#include <error.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "core/lv_group.h"
#include "misc/lv_event.h"
#include "ui/display.h"
#include "ui/screen.h"
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

  struct UiDisplay display;
  struct UiScreen screen;
};

static void ui_menu_book_event_cb(lv_event_t *e);
static void ui_reader_book_event_cb(lv_event_t *e, book_t, ui_t);

err_t ui_init(ui_t *out,
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

  err_o = ui_display_init(&ui->display, ui, settings_display_model);
  ERR_TRY(err_o);

  FILE *boot_screen_fd = fopen(settings_boot_screen_path, "r");
  if (!boot_screen_fd) {
    err_o = err_errnof(ENOENT, "File does not exist: %s",
                       settings_boot_screen_path);
    goto error_display_cleanup;
  }

  unsigned char *img_buf = mem_malloc(48000 + 8);
  const size_t ret_code = fread(img_buf, 1, 48000, boot_screen_fd);
  if (ret_code != 48000) {
    err_o =
        err_errnof(ENOENT, "Cannot read file %s", settings_boot_screen_path);
    goto error_boot_screen_fd_cleanup;
  }
  fclose(boot_screen_fd);

  err_o = ui_display_render(&ui->display, img_buf, 48000);
  ERR_TRY_CATCH(err_o, error_boot_screen_cleanup);
  mem_free(img_buf);

  return 0;

error_boot_screen_fd_cleanup:
  fclose(boot_screen_fd);
error_boot_screen_cleanup:
  mem_free(img_buf);
error_display_cleanup:
  ui_display_destroy(&ui->display);
error_out:
  mem_free(*out);
  *out = NULL;
  return err_o;
}

int ui_tick(ui_t ui) {
  int time = lv_timer_handler();
  return time;
  /* (void)  time; */
  /* return 1000; */
};

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
ui_t gui;
err_t ui_menu_init(ui_t ui, books_list_t books, int book_i) {
  puts(__func__);
  lv_group_t *group = ui_display_get_input_group(&ui->display);
  err_o = ui_screen_menu_init(&ui->screen, ui, books, book_i, LV_EVENT_KEY,
                              ui_menu_book_event_cb, group);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

void ui_menu_destroy(ui_t ui) {
  puts(__func__);
  ui_screen_destroy(&ui->screen);
};

void ui_panic(ui_t ui) { ui_display_panic(&ui->display); };

static void ui_menu_book_event_cb(lv_event_t *e) {
  ui_wx_menu_book_t book = lv_event_get_user_data(e);
  ui_t ui = ui_wx_menu_book_get_ui(book);
  lv_key_t key = lv_event_get_key(e);

  log_debug("Ui received key: %d'", key);

  if (key == '\r' || key == '\n') {
    key = LV_KEY_ENTER;
  }

  if (key == LV_KEY_ENTER) {
    int *id = mem_malloc(sizeof(int));
    *id = ui_wx_menu_book_get_id(book);
    ui->inputh.callback(UiInputEventEnum_ENTER, ui->inputh.data, id);
  }

  if (key == LV_KEY_ESC) {
    ui->inputh.callback(UiInputEventEnum_MENU, ui->inputh.data, NULL);    
  }    
}

err_t ui_reader_init(ui_t ui, book_t book) {
  err_o = ui_screen_reader_init(&ui->screen, ui, book, LV_EVENT_KEY,
                                ui_reader_book_event_cb,
                                ui_display_get_input_group(&ui->display));
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

void ui_reader_destroy(ui_t ui) {
  puts(__func__);
  ui_screen_destroy(&ui->screen);
};

static void ui_reader_book_event_cb(lv_event_t *e, book_t book, ui_t ui) {
  lv_key_t key = lv_event_get_key(e);

  if (key == '\r' || key == '\n' || key == LV_KEY_ENTER) {
    ui->inputh.callback(UiInputEventEnum_ENTER, ui->inputh.data, book);
  } else if (key == LV_KEY_LEFT) {
    ui->inputh.callback(UiInputEventEnum_LEFT, ui->inputh.data, book);
  } else if (key == LV_KEY_RIGHT) {
    ui->inputh.callback(UiInputEventEnum_RIGTH, ui->inputh.data, book);
  } else if (key == LV_KEY_UP) {
    ui->inputh.callback(UiInputEventEnum_UP, ui->inputh.data, book);
  } else if (key == LV_KEY_DOWN) {
    ui->inputh.callback(UiInputEventEnum_DOWN, ui->inputh.data, book);
  }

  else if (key == LV_KEY_ESC) {
    ui->inputh.callback(UiInputEventEnum_MENU, ui->inputh.data, book);    
  }    
  }

err_t ui_reader_settings_init(ui_t ui, const char **fields, int fields_len){    
  err_o = ui_screen_reader_settings_init(&ui->screen, fields,fields_len,  LV_EVENT_KEY,
                                ui_reader_book_event_cb,
                                ui_display_get_input_group(&ui->display));
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}
  
/* void ui_reader_book_event_cb______(lv_event_t *e) { */
/*   book_t book = lv_event_get_user_data(e); */
/*   lv_key_t key = lv_event_get_key(e); */

/*   log_debug("Ui received key: %d'", key); */

/*   if (key == '\r' || key == '\n') { */
/*     key = LV_KEY_ENTER; */
/*   } */

/*   static int x_offset = 0; */
/*   static int y_offset = 0; */
/*   static double scale = 1; */

/*   if (key == 17) { */
/*     y_offset -= 25; */
/*     book_set_y_offset(book, y_offset); */
/*     ui_screen_destroy(&gui->screen); */
/*     ui_screen_reader_init(&gui->screen, gui, book, LV_EVENT_ALL, */
/*                           ui_reader_book_event_cb, group); */
/*   } */

/*   if (key == 18) { */
/*     y_offset += 25; */
/*     book_set_y_offset(book, y_offset); */
/*     ui_screen_destroy(&gui->screen); */
/*     ui_screen_reader_init(&gui->screen, gui, book, LV_EVENT_ALL, */
/*                           ui_reader_book_event_cb, group); */
/*   } */

/*   if (key == 19) { */
/*     x_offset += 25; */
/*     book_set_x_offset(book, x_offset); */
/*     ui_screen_destroy(&gui->screen); */
/*     ui_screen_reader_init(&gui->screen, gui, book, LV_EVENT_ALL, */
/*                           ui_reader_book_event_cb, group); */
/*   } */

/*   if (key == 20) { */
/*     x_offset -= 25; */
/*     book_set_x_offset(book, x_offset); */
/*     ui_screen_destroy(&gui->screen); */
/*     ui_screen_reader_init(&gui->screen, gui, book, LV_EVENT_ALL, */
/*                           ui_reader_book_event_cb, group); */
/*   } */

/*   if (key == 43) { */
/*     scale += 0.1; */
/*     book_set_scale(book, scale); */
/*     ui_screen_destroy(&gui->screen); */
/*     ui_screen_reader_init(&gui->screen, gui, book, LV_EVENT_ALL, */
/*                           ui_reader_book_event_cb, group); */
/*   } */
/*   if (key == 45) { */
/*     scale -= 0.1; */
/*     book_set_scale(book, scale); */
/*     ui_screen_destroy(&gui->screen); */
/*     ui_screen_reader_init(&gui->screen, gui, book, LV_EVENT_ALL, */
/*                           ui_reader_book_event_cb, group); */
/*   } */
/* } */
