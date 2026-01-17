#include <stdio.h>

#include "app/core.h"
#include "app/state.h"
#include "book/book.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct AppMenu *app_state_menu_t;

struct AppMenu {
  app_t owner;
  ui_t ui;
  int current_book_i;
  books_list_t blist;
};

static void app_state_menu_open(void *, app_ctx_t, void *);
static void app_state_menu_close(void *);
static void app_state_menu_destroy(void *);

err_t app_state_menu_create(app_state_t *out, app_t app) {
  app_state_menu_t menu = mem_malloc(sizeof(struct AppMenu));
  *menu = (struct AppMenu){
      .owner = app,
  };

  err_o = app_state_create(out, app_state_menu_open, app_state_menu_close,
                           app_state_menu_destroy, menu);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

static void app_state_menu_open(void *state, app_ctx_t ctx, void *arg) {
  app_state_menu_t menu = state;

  menu->blist = book_api_find_books(ctx->book_api);
  menu->current_book_i = 0;
  menu->ui = ctx->ui;

  if (menu->blist == NULL) { // We should display sth wich would indicate
                             // lack of books instead of raising error.
    err_o = err_errnos(ENOENT, "No books");
    goto error_out;
  }

  err_o = ui_menu_create(ctx->ui, menu->blist, menu->current_book_i);
  ERR_TRY_CATCH(err_o, error_blist_cleanup);

  ui_render_cleanup(ctx->ui);

  return;

error_blist_cleanup:
  books_list_destroy(menu->blist);
error_out:
  app_raise_error(menu->owner, err_o);
}

static void app_state_menu_close(void *state) {
  app_state_menu_t menu = state;

  if (menu->ui) {
    ui_menu_destroy(menu->ui);
    menu->ui = NULL;
  }
  if (menu->blist) {
    books_list_destroy(menu->blist);
    menu->blist = NULL;
  }
};

static void app_state_menu_destroy(void *state) {
  puts(__func__);
  app_state_menu_close(state);
  mem_free(state);
  state = NULL;
};

/**
   @todo Instead of NULL add book. Propably need sth like list_pop to receive
   book.
 */
void app_state_menu_select_book(app_state_t astate, app_ctx_t app, void *__) {
  app_state_menu_t menu = app_state_get_private(astate);

  app_event_post(menu->owner, AppEventEnum_BOOK_SELECTED, NULL);
}
