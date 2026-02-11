#include <lvgl.h>

#include "app/app.h"
#include "display/display.h"
#include "event_bus/event_bus.h"
#include "library/library.h"
#include "menu/menu.h"
#include "menu_screen/menu_screen.h"
#include "misc/lv_timer.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/time.h"

struct App {
  menu_screen_t menu_screen;
  library_t library;
  display_t display;
  menu_t menu;
  bus_t bus;
};

err_t app_init(app_t *out) {
  app_t app = *out = mem_malloc(sizeof(struct App));
  *app = (struct App){0};

  lv_init();
  event_bus_init(&app->bus);

  err_o = display_init(&app->display);
  ERR_TRY(err_o);

  err_o = library_init(&app->library);
  ERR_TRY(err_o);

  err_o = menu_init(&app->menu, app->library, app->bus);
  ERR_TRY(err_o);

  err_o = menu_screen_init(&app->menu_screen, app->display, app->bus);
  ERR_TRY(err_o);

  event_bus_post_event(app->bus, BusEnum_ALL,
                       (struct Event){
                           .event = EventEnum_BOOT_COMPLETED,
                       });

  return 0;

error_out:
  app_destroy(out);
  return err_o;
}

void app_destroy(app_t *out) {
  if (!out || !*out) {
    return;
  }

  app_t app = *out;

  if (app->menu_screen) {
    menu_screen_destroy(&app->menu_screen);
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

  if (app->bus) {
    event_bus_destroy(&app->bus);
  }

  mem_free(*out);
  *out = NULL;
};

err_t app_main(app_t app) {
  int sleep_ms = 0;
  while (1) {
    event_bus_step(app->bus);

    sleep_ms = lv_timer_handler();
    time_sleep_ms(sleep_ms);
  }
  return 0;
};

void app_panic(app_t app) { (void)app; }
