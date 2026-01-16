#include "app/core.h"
#include "book/book.h"
#include "ui/widgets.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct AppMenu *app_menu_t;

struct AppMenu {
  app_t owner;
  ui_t ui;
  int current_book_i;
  books_list_t blist;
};

static void app_menu_open(app_module_t, app_ctx_t, void *);
static void app_menu_close(app_module_t);
static void app_menu_destroy(app_module_t);

err_t app_menu_init(app_module_t module, app_t app) {
  app_menu_t menu = mem_malloc(sizeof(struct AppMenu));
  *menu = (struct AppMenu){
      .owner = app,
  };

  module->open = app_menu_open;
  module->close = app_menu_close;
  module->destroy = app_menu_destroy;
  module->private = menu;

  return 0;
};

static void app_menu_open(app_module_t module, app_ctx_t ctx, void *arg) {
  app_menu_t menu = module->private;

  menu->blist = book_api_find_books(ctx->book_api);
  menu->current_book_i = 0;

  if (menu->blist == NULL) { // We should display sth wich would indicate
                             // lack of books instead of raising error.
    err_o = err_errnos(ENOENT, "No books");
    goto error_out;
  }

  err_o = ui_menu_create(ctx->ui, menu->blist, menu->current_book_i);
  ERR_TRY_CATCH(err_o, error_blist_cleanup);

  return;

error_blist_cleanup:
  books_list_destroy(menu->blist);
error_out:
  app_raise_error(menu->owner, err_o);
}

static void app_menu_close(app_module_t module) {
  app_menu_t menu = module->private;

  if (menu->ui) {
    ui_menu_destroy(menu->ui);
    menu->ui = NULL;
  }
  if (menu->blist) {
    books_list_destroy(menu->blist);
    menu->blist = NULL;
  }
};

static void app_menu_destroy(app_module_t module) {
  if (!module->private) {

    return;
  }

  app_menu_close(module);
  mem_free(module->private);
  module->private = NULL;
};

/**
   @todo Instead of NULL add book. Propably need sth like list_pop to receive book.
 */
void app_menu_select_book(app_module_t module, app_ctx_t app, void *arg) {
  app_menu_t menu = module->private;
  /* int *book_i = arg; */
  /* menu->current_book_i = *book_i; */
  
  app_event_post(menu->owner, AppEventEnum_BOOK_SELECTED, NULL);
}
