#include "app/core.h"
#include "app/module.h"
#include "book/book.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/time.h"
#include "utils/zlist.h"

struct App {
  struct AppModule modules[AppStateEnum_MAX];
  enum AppStateEnum current_state;
  struct ZList ev_queue;
  struct AppCtx ctx;
  bool is_running;
};

struct AppFsmTransition {
  enum AppStateEnum next_state;
  /**
     @brief Action that need to be performed upon transition to next_state.

     Action does not return error, because once app is running all errors should
     be reported via app_error_raise.
   */
  void (*action)(app_module_t, app_ctx_t, enum AppEventEnum, void *);
};

static const struct AppFsmTransition
    fsm_table[AppStateEnum_MAX][AppEventEnum_MAX] = {
        [AppStateEnum_BOOT] =
            {
                [AppEventEnum_BOOT_DONE] =
                    {
                        .next_state = AppStateEnum_MENU,
                    },
                [AppEventEnum_ERROR_RAISED] =
                    {
                        .next_state = AppStateEnum_ERROR,
                    },
            },
        [AppStateEnum_MENU] =
            {
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppStateEnum_MENU,
                        .action = app_module_menu_select_book,
                    },

                [AppEventEnum_BOOK_SELECTED] =
                    {
                        .next_state = AppStateEnum_READER,
                    },

                [AppEventEnum_ERROR_RAISED] =
                    {
                        .next_state = AppStateEnum_ERROR,
                    },
            },
        [AppStateEnum_READER] = {
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppStateEnum_READER,
                    },            
                [AppEventEnum_BTN_UP] =
                    {
                        .next_state = AppStateEnum_READER,
                    },
                [AppEventEnum_BTN_DOWN] =
                    {
                        .next_state = AppStateEnum_READER,
                    },
                [AppEventEnum_BTN_LEFT] =
                    {
                        .next_state = AppStateEnum_READER,
                    },
                [AppEventEnum_BTN_RIGTH] =
                    {
                        .next_state = AppStateEnum_READER,
                    },
                [AppEventEnum_BTN_MENU] =
                    {
                        .next_state = AppStateEnum_MENU,
                    },
                [AppEventEnum_ERROR_RAISED] =
                    {
                        .next_state = AppStateEnum_ERROR,
                    },
            },
};

static void app_input_callback(enum UiInputEventEnum event, void *data,
                               void *arg);
static const char *app_state_dump(enum AppStateEnum);
static void app_step(app_t);
static void app_ev_queue_push(app_t, app_event_t);
static app_event_t app_ev_queue_pull(app_t);

err_t app_init(app_t *out) {
  app_t app = *out = mem_malloc(sizeof(struct App));
  *app = (struct App){
      .current_state = AppStateEnum_BOOT,
  };

  err_o = ui_init(&app->ctx.ui, app_input_callback, app);
  ERR_TRY(err_o);

  err_o = book_api_init(&app->ctx.book_api);
  ERR_TRY_CATCH(err_o, error_ui_cleanup);

  int inits_status;
  for (inits_status = AppStateEnum_MENU; inits_status < AppStateEnum_MAX;
       inits_status++) {
    err_o = app_module_init(&app->modules[inits_status], app, inits_status);
    ERR_TRY_CATCH(err_o, error_modules_cleanup);
  }

  app_event_post(app, AppEventEnum_BOOT_DONE, NULL);

  return 0;

error_modules_cleanup:
  app_modules_destroy((*out)->modules, inits_status);
  book_api_destroy(&app->ctx.book_api);
error_ui_cleanup:
  ui_destroy(&app->ctx.ui);
error_out:
  mem_free(*out);
  *out = NULL;
  return err_o;
};

err_t app_main(app_t app) {
  int sleeping_time;

  app->is_running = true;
  while (app->is_running) {
    app_step(app);

    sleeping_time = ui_tick(app->ctx.ui);
    time_sleep_ms(sleeping_time);
  }

  return 0;
}

void app_destroy(app_t *out) {
  if (!out || !*out) {
    return;
  }

  app_module_close(&(*out)->modules[(*out)->current_state]);
  app_modules_destroy((*out)->modules, AppStateEnum_MAX);
  book_api_destroy(&(*out)->ctx.book_api);
  ui_destroy(&(*out)->ctx.ui);
  mem_free(*out);
  *out = NULL;
};


void app_event_post(app_t app, enum AppEventEnum event, void *data) {
  app_event_t ev = mem_malloc(sizeof(struct AppEventData));
  *ev = (struct AppEventData){
      .event = event,
      .data = data,
  };

  app_ev_queue_push(app, ev);
}

/**
   @brief Move app to next state according to fsm_table.
   @note We clean app->ev_data before performing step action,
         so every step start with clean event and event data.
*/
static void app_step(app_t app) {
  struct AppFsmTransition trans;
  app_module_t current_module;
  app_module_t next_module;
  app_event_t event;

  while (app->ev_queue.len != 0) {
    event = app_ev_queue_pull(app);
    trans = fsm_table[app->current_state][event->event];

    log_debug("%s -> %s, event=%s", app_state_dump(app->current_state),
              app_state_dump(trans.next_state), app_event_dump(event->event));

    next_module = &app->modules[trans.next_state];
    current_module = &app->modules[app->current_state];

    if (app->current_state != trans.next_state) {
      app_module_close(current_module);
    }

    if (!trans.action) {
      trans.action = app_module_open;
    }

    trans.action(next_module, &app->ctx, event->event, event->data);
    app->current_state = trans.next_state;
    
    mem_free(event);
  }
}

void app_raise_error(app_t app, err_t error) {
  app_event_post(app, AppEventEnum_ERROR_RAISED, error);
}

void app_panic(app_t app) { ui_panic(app->ctx.ui); }

const char *app_event_dump(enum AppEventEnum event) {
  static const char *event_str_map[] = {
      [AppEventEnum_NONE] = "ev_none",
      [AppEventEnum_BOOT_DONE] = "ev_boot_done",
      [AppEventEnum_BTN_ENTER] = "ev_btn_enter",
      [AppEventEnum_BTN_MENU] = "ev_btn_menu",
      [AppEventEnum_BTN_LEFT] = "ev_btn_left",
      [AppEventEnum_BTN_RIGTH] = "ev_btn_right",
      [AppEventEnum_BTN_UP] = "ev_btn_up",
      [AppEventEnum_BTN_DOWN] = "ev_btn_down",
      [AppEventEnum_BOOK_SELECTED] = "ev_book_selected",
      [AppEventEnum_ERROR_RAISED] = "ev_error_raised",
  };

  if (event < AppEventEnum_NONE || event > AppEventEnum_ERROR_RAISED ||
      !event_str_map[event]) {
    return "Unknown";
  }

  return event_str_map[event];
};

static const char *app_state_dump(enum AppStateEnum state) {
  static char *dumps[] = {
      [AppStateEnum_MENU] = "state_menu",
      [AppStateEnum_BOOT] = "state_boot",
      [AppStateEnum_READER] = "state_book_reader",
      [AppStateEnum_ERROR] = "state_error",
  };

  if (state < AppStateEnum_MENU || state > AppStateEnum_ERROR ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void app_input_callback(enum UiInputEventEnum event, void *data,
                               void *arg) {
  static enum AppEventEnum gui_input_ev_table[] = {
      [UiInputEventEnum_UP] = AppEventEnum_BTN_UP,
      [UiInputEventEnum_DOWN] = AppEventEnum_BTN_DOWN,
      [UiInputEventEnum_LEFT] = AppEventEnum_BTN_LEFT,
      [UiInputEventEnum_RIGTH] = AppEventEnum_BTN_RIGTH,
      [UiInputEventEnum_ENTER] = AppEventEnum_BTN_ENTER,
      [UiInputEventEnum_MENU] = AppEventEnum_BTN_MENU,
  };

  app_t app = data;

  app_event_post(app, gui_input_ev_table[event], arg);
}

static void app_ev_queue_push(app_t app, app_event_t event) {
  zlist_append(&app->ev_queue, &event->next);
}

static app_event_t app_ev_queue_pull(app_t app) {
  zlist_node_t node = zlist_pop(&app->ev_queue, 0);
  if (!node) {
    return NULL;
  }

  return mem_container_of(node, struct AppEventData, next);
};

