#include "app/core.h"
#include "book/book.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct AppMenu *app_menu_t;

struct AppMenu {
  app_t owner;
  ui_t ui;
  int current_book_i;
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

  return 0;
};

static void app_menu_open(app_module_t module, app_ctx_t ctx, void *arg) {
  books_list_t blist = book_api_find_books(ctx->book_api);
  app_menu_t menu = module->private;
  menu->current_book_i = 0;

  if (blist == NULL){
    err_errnos(ENOENT, "No books");
    goto error_out;
    }

  err_errno = ui_menu_create(ctx->ui, blist, menu->current_book_i);
  ERR_TRY_CATCH(err_errno, error_blist_cleanup);

    
  books_list_destroy(blist);

  return;

 error_blist_cleanup:
  books_list_destroy(blist);
error_out:
  app_raise_error(menu->owner, err_errno);
}

static void app_menu_close(app_module_t module) {

};

static void app_menu_destroy(app_module_t module) {

};
