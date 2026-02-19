#include <lvgl.h>

#include "app/app.h"
#include "book_settings/book_settings.h"
#include "db/db.h"
#include "display/display.h"
#include "event_queue/event_queue.h"
#include "library/library.h"
#include "menu/menu.h"
#include "reader/reader.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/time.h"

struct App {
  book_settings_t book_settings;
  event_queue_t event_queue;
  display_t display;
  library_t library;
  reader_t reader;
  menu_t menu;
  db_t db;
};

err_t app_init(app_t *out) {
  app_t app = *out = mem_malloc(sizeof(struct App));
  *app = (struct App){0};

  event_queue_init(&app->event_queue);

  lv_init();
  err_o = display_init(&app->display);
  ERR_TRY(err_o);

  err_o = db_init(&app->db);
  ERR_TRY(err_o);

  err_o = library_init(&app->library, app->db);
  ERR_TRY(err_o);

  err_o = menu_init(&app->menu, app->display, app->event_queue, app->library);
  ERR_TRY(err_o);

  err_o =
      reader_init(&app->reader, app->display, app->event_queue, app->library);
  ERR_TRY(err_o);

  err_o = book_settings_init(&app->book_settings, app->display,
                             app->event_queue, app->library);
  ERR_TRY(err_o);

  event_queue_push(app->event_queue, Events_BOOT_DONE, NULL);

  return 0;

error_out:
  app_destroy(out);
  return err_o;
};

void app_destroy(app_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  app_t app = *out;

  if (app->book_settings) {
    book_settings_destroy(&app->book_settings);
  }

  if (app->reader) {
    reader_destroy(&app->reader);
  }

  if (app->menu) {
    menu_destroy(&app->menu);
  }

  if (app->library) {
    library_destroy(&app->library);
  }

  if (app->display) {
    display_destroy(&app->display);
  }

  if (app->event_queue) {
    event_queue_destroy(&app->event_queue);
  }

  if (app->db) {
    db_destroy(&app->db);
  }

  mem_free(app);
  *out = NULL;
};

err_t app_main(app_t app) {
  while (1) {
    event_queue_step(app->event_queue);

    int ms = lv_timer_handler();
    time_sleep_ms(ms);
  }
};
