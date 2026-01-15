#include "core.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"

enum AppStateEnum {
  AppStateEnum_NONE = 0,
  AppStateEnum_BOOT,
  AppStateEnum_MENU,
  AppStateEnum_READER,
  AppStateEnum_ERROR,
  // Add more states here
  AppStateEnum_MAX,
};

struct App {
  struct AppModule modules[AppStateEnum_MAX];
  struct AppEventData ev_data;
  enum AppStateEnum state;
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
  void (*action)(app_module_t, app_ctx_t, void *);
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
                // In menu we use LVGL flex layout which can handle
                // Changes in selection on it's own, so we just receive
                // Info that book is selected.
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppStateEnum_MENU,
                        /* .action = ebk_corem_menu_select_book, */
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
        [AppStateEnum_READER] =
            {
                [AppEventEnum_BTN_LEFT] =
                    {
                        .next_state = AppStateEnum_READER,
                        /* .action = ebk_corem_reader_prev_page, */
                    },
                [AppEventEnum_BTN_RIGTH] =
                    {
                        .next_state = AppStateEnum_READER,
                        /* .action = ebk_corem_reader_next_page, */
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

static const char *app_state_dump(enum AppStateEnum state);
static void app_step(app_t app);

err_t app_init(app_t *out) {
  app_t app = *out = mem_malloc(sizeof(struct App));
  *app = (struct App){
      .state = AppStateEnum_BOOT,
  };

  err_errno = ui_init(&app->ctx.ui);
  ERR_TRY(err_errno);

  (void)app_step;

  (void)fsm_table;
  return 0;

error_out:
  mem_free(*out);
  *out = NULL;
  return err_errno;
};

err_t app_main(app_t app) {
  while (1) {
    
      }
  return 0; }
void app_destroy(app_t *out) {
  if (!out || !*out) {
    return;
  }

  mem_free(*out);
  *out = NULL;
};

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

void app_event_post(app_t app, enum AppEventEnum event, void *data) {

  if (!fsm_table[app->state][event].next_state) {
    log_warn("Requested unsupported transition: %s:%s -> STATE_NONE",
             app_state_dump(app->state), app_event_dump(event));
    goto out;
  }

  // Error cannot be overwritten
  if (app->ev_data.event == AppEventEnum_ERROR_RAISED) {
    goto out;
  }

  // Every other event can but we should try to do not allow
  // for such situations because they can lead to missing events.
  // If there will be a need for multiple events better to do
  // some list for events than to overwrite them, but idk if this
  // issue will arise. Will see when the code wiull be used for
  // some time.
  if (app->ev_data.event) {
    log_warn("Overwriting existing event %s with event %s",
             app_state_dump(app->state), app_event_dump(event));
  }

  app->ev_data.event = event;
  app->ev_data.data = data;

out:;
}

static const char *app_state_dump(enum AppStateEnum state) {
  static char *dumps[] = {
      [AppStateEnum_NONE] = "state_none",
      [AppStateEnum_MENU] = "state_menu",
      [AppStateEnum_BOOT] = "state_boot",
      [AppStateEnum_READER] = "state_book_reader",
      [AppStateEnum_ERROR] = "state_error",
  };

  if (state < AppStateEnum_NONE || state > AppStateEnum_ERROR ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

/**
   @brief Move app to next state according to fsm_table.
   @note We clean app->ev_data before performing step action,
         so every step start with clean event and event data.
*/
static void app_step(app_t app) {
  if (!app->ev_data.event) {
    goto out;
  }

  struct AppEventData ev_data = app->ev_data;
  memset(&app->ev_data, 0, sizeof(struct AppEventData));

  struct AppFsmTransition trans = fsm_table[app->state][ev_data.event];
  app_module_t next_module = &app->modules[trans.next_state];
  app_module_t current_module = &app->modules[app->state];

  log_debug("%s -> %s", app_state_dump(app->state),
            app_state_dump(trans.next_state));

  if (!trans.action && next_module->open) {
    trans.action = next_module->open;
  }

  trans.action(next_module, &app->ctx, ev_data.data);

  if (app->state != trans.next_state && current_module->close) {
    current_module->close(current_module);
  }

  app->state = trans.next_state;

out:;
}
