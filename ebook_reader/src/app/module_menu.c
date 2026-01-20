#include <assert.h>
#include <stdio.h>

#include "app/core.h"
#include "app/module.h"
#include "book/book.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct AppMenu *app_module_menu_t;

struct AppMenu {
  app_t owner;
  ui_t ui;
  int current_book_i;
  books_list_t blist;
};

static void app_module_menu_open(void *, app_ctx_t, void *);
static void app_module_menu_close(void *);
static void app_module_menu_destroy(void *);

err_t app_module_menu_create(app_module_t *out, app_t app) {
  app_module_menu_t menu = mem_malloc(sizeof(struct AppMenu));
  *menu = (struct AppMenu){
      .owner = app,
  };

  err_o = app_module_create(out, app_module_menu_open, app_module_menu_close,
                            app_module_menu_destroy, menu);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

/**
   @todo We should display sth wich would indicate lack of books instead of
   raising error.
*/
static void app_module_menu_open(void *module, app_ctx_t ctx, void *__) {
  assert(module != NULL);
  assert(ctx->book_api != NULL);

  app_module_menu_t menu = module;

  menu->blist = book_api_find_books(ctx->book_api);
  menu->current_book_i = 0;
  menu->ui = ctx->ui;

  if (menu->blist == NULL) {
    ERR_TRY(err_o);
  }

  err_o = ui_menu_create(ctx->ui, menu->blist, menu->current_book_i);
  ERR_TRY_CATCH(err_o, error_blist_cleanup);

  return;

error_blist_cleanup:
  books_list_destroy(menu->blist);
error_out:
  app_raise_error(menu->owner, err_o);
}

static void app_module_menu_close(void *module) {
  app_module_menu_t menu = module;

  if (menu->ui) {
    ui_menu_destroy(menu->ui);
    menu->ui = NULL;
  }
  if (menu->blist) {
    books_list_destroy(menu->blist);
    menu->blist = NULL;
  }
};

static void app_module_menu_destroy(void *module) {
  puts(__func__);
  app_module_menu_close(module);
  mem_free(module);
};

/**
   @todo Instead of NULL add book. Propably need sth like list_pop to receive
   book.
 */
void app_module_menu_select_book(app_module_t module, app_ctx_t __, void *___) {
  app_module_menu_t menu = app_module_get_module_data(module);

  app_event_post(menu->owner, AppEventEnum_BOOK_SELECTED, NULL);
}
