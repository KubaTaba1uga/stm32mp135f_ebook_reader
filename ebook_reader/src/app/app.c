#include "app/app.h"
#include "display/display.h"
#include "menu/menu.h"
#include "utils/err.h"
#include "utils/mem.h"

struct App {
  display_t display;
  menu_t menu;
};

err_t app_init(app_t *out) {
  app_t app = *out = mem_malloc(sizeof(struct App));
  
  err_o = display_init(&app->display);
  ERR_TRY(err_o);
  
  err_o = menu_init(&app->menu, app->display);
  ERR_TRY(err_o);

  
  

  return 0;

error_out:
  return err_o;
}

err_t app_main(app_t app) { return 0; };

void app_destroy(app_t *out) {
  if (!out || !*out) {
    return;
  }

  mem_free(*out);
  *out = NULL;
};

void app_panic(app_t app) { (void)app; }
