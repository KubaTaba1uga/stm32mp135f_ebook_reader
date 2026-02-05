#include "app/core.h"
#include "app/module.h"
#include "book/book.h"
#include "ui/ui.h"
#include "utils/log.h"
#include "utils/mem.h"

typedef struct AppReader *app_module_reader_t;

enum AppReaderStateEnum {
  AppReaderStateEnum_NONE = 0,
  AppReaderStateEnum_PAGE,
  AppReaderStateEnum_SETTINGS,
  AppReaderStateEnum_SET_ZOOM,
  AppReaderStateEnum_SET_X_OFF,
  AppReaderStateEnum_SET_Y_OFF,
  AppReaderStateEnum_MAX,
};

struct AppReader {
  enum AppReaderStateEnum state;
  book_t book;
  app_t owner;
  ui_t ui;
};

struct AppReaderFsmTransition {
  enum AppReaderStateEnum next_state;
  void (*action)(app_module_reader_t, app_ctx_t, enum AppEventEnum, void *);
};

static const char *fields[] = {
    "Set zoom",
    "Set X offset",
    "Set Y offset",
    "Back",
};

// Generic API
static const char *app_reader_state_dump(enum AppReaderStateEnum);
static void app_module_reader_close(void *);
static void app_module_reader_destroy(void *);

// Page API
static void app_module_reader_page_open(app_module_reader_t, app_ctx_t,
                                        enum AppEventEnum, void *);
static void app_module_reader_page_prev(app_module_reader_t, app_ctx_t,
                                        enum AppEventEnum, void *);
static void app_module_reader_page_next(app_module_reader_t, app_ctx_t,
                                        enum AppEventEnum, void *);
static void app_module_reader_step(void *, app_ctx_t, enum AppEventEnum,
                                   void *);
static void app_module_reader_page_closed(app_module_reader_t);

// Settings API
static void app_module_reader_settings_open(app_module_reader_t, app_ctx_t,
                                            enum AppEventEnum, void *);
static void app_module_reader_settings_select(app_module_reader_t, app_ctx_t,
                                              enum AppEventEnum, void *);

struct AppReaderFsmTransition
    reader_fsm_table[AppReaderStateEnum_MAX][AppEventEnum_MAX] = {
        [AppReaderStateEnum_NONE] =
            {
                [AppEventEnum_BOOK_SELECTED] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_page_open,
                    },
            },
        [AppReaderStateEnum_PAGE] =
            {
                [AppEventEnum_BTN_LEFT] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_page_prev,
                    },
                [AppEventEnum_BTN_RIGTH] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_page_next,
                    },
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppReaderStateEnum_SETTINGS,
                        .action = app_module_reader_settings_open,
                    },
            },
        [AppReaderStateEnum_SETTINGS] =
            {
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppReaderStateEnum_SETTINGS,
                        .action = app_module_reader_settings_select,
                    },
            },
};

err_t app_module_reader_init(app_module_t out, app_t app) {
  app_module_reader_t reader = mem_malloc(sizeof(struct AppReader));
  *reader = (struct AppReader){
      .owner = app,
  };

  *out = (struct AppModule){
      .open = app_module_reader_step,
      .close = app_module_reader_close,
      .destroy = app_module_reader_destroy,
      .module_data = reader,
  };

  return 0;
};

static void app_module_reader_step_priv(struct AppReaderFsmTransition *trans,
                                        void *module, app_ctx_t ctx,
                                        enum AppEventEnum event, void *arg) {
  app_module_reader_t reader = module;

  if (!trans->action) {
    log_debug("Triggered transaction without action: event=%s, state=%s",
              app_event_dump(event), app_reader_state_dump(reader->state));
    goto out;
  }

  log_debug("%s -> %s, event=%s", app_reader_state_dump(reader->state),
            app_reader_state_dump(trans->next_state), app_event_dump(event));

  trans->action(reader, ctx, event, arg);
  reader->state = trans->next_state;

out:;
}

static void app_module_reader_step(void *module, app_ctx_t ctx,
                                   enum AppEventEnum event, void *arg) {
  app_module_reader_t reader = module;
  struct AppReaderFsmTransition trans;

  trans = reader_fsm_table[reader->state][event];

  app_module_reader_step_priv(&trans, reader, ctx, event, arg);
}

static void app_module_reader_close(void *module) {
  app_module_reader_t reader = module;

  switch (reader->state) {
  case AppReaderStateEnum_PAGE:
    app_module_reader_page_closed(reader);
    break;
  default:;
  }

  reader->state = AppReaderStateEnum_NONE;
  ui_reader_destroy(reader->ui);

  reader->ui = NULL;
};

static void app_module_reader_destroy(void *module) { mem_free(module); };

static void app_module_reader_page_open(app_module_reader_t reader,
                                        app_ctx_t ctx, enum AppEventEnum event,
                                        void *arg) {
  book_t book = arg;

  err_o = ui_reader_init(ctx->ui, book);
  ERR_TRY(err_o);

  reader->ui = ctx->ui;
  reader->book = book;

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_page_closed(app_module_reader_t reader) {
  ui_reader_destroy(reader->ui);
  book_destroy(&reader->book);
};

static const char *app_reader_state_dump(enum AppReaderStateEnum state) {
  static char *dumps[AppReaderStateEnum_MAX] = {
      [AppReaderStateEnum_PAGE] = "state_page",
      [AppReaderStateEnum_SETTINGS] = "state_settings",
      [AppReaderStateEnum_SET_ZOOM] = "state_zoom",
      [AppReaderStateEnum_SET_X_OFF] = "state_set_x_off",
      [AppReaderStateEnum_SET_Y_OFF] = "state_set_y_off",
  };

  if (state <= AppReaderStateEnum_NONE || state > AppReaderStateEnum_MAX ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

static void app_module_reader_page_prev(app_module_reader_t reader,
                                        app_ctx_t ctx, enum AppEventEnum event,
                                        void *arg) {
  int page_no = book_get_page_no(reader->book);
  page_no--;
  book_set_page_no(reader->book, page_no);

  ui_reader_destroy(ctx->ui);
  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_page_next(app_module_reader_t reader,
                                        app_ctx_t ctx, enum AppEventEnum event,
                                        void *arg) {
  int page_no = book_get_page_no(reader->book);
  page_no++;
  book_set_page_no(reader->book, page_no);

  ui_reader_destroy(ctx->ui);
  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_settings_open(app_module_reader_t reader,
                                            app_ctx_t ctx,
                                            enum AppEventEnum event,
                                            void *arg) {

  err_o =
      ui_reader_settings_init(ctx->ui, fields, sizeof(fields) / sizeof(char *));
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_zoom_open(app_module_reader_t reader,
                                            app_ctx_t ctx,
                                            enum AppEventEnum event, void *arg);
static void app_module_reader_set_x_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg);
static void app_module_reader_set_y_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg);
static void app_module_reader_settings_select(app_module_reader_t reader,
                                              app_ctx_t ctx,
                                              enum AppEventEnum event,
                                              void *arg) {
  static struct AppReaderFsmTransition
      settings_fsm_table[sizeof(fields) / sizeof(char *)] = {
          {
              .next_state = AppReaderStateEnum_SET_ZOOM,
              .action = app_module_reader_set_zoom_open,
          },
          {
              .next_state = AppReaderStateEnum_SET_X_OFF,
              .action = app_module_reader_set_x_off_open,
          },
          {
              .next_state = AppReaderStateEnum_SET_Y_OFF,
              .action = app_module_reader_set_y_off_open,
          },
          {
              .next_state = AppReaderStateEnum_PAGE,
              .action = app_module_reader_page_open,
          },
      };
  int *id = arg;

  struct AppReaderFsmTransition trans = settings_fsm_table[*id];

  if (*id >= sizeof(fields) / sizeof(char *) || !trans.action ||
      !trans.next_state) {
    return;
  }

  app_module_reader_step_priv(&trans, reader, ctx, event, arg);
  ui_reader_settings_destroy(reader->ui);
}

static void app_module_reader_set_zoom_open(app_module_reader_t reader,
                                            app_ctx_t ctx,
                                            enum AppEventEnum event,
                                            void *arg) {}
static void app_module_reader_set_x_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg) {}
static void app_module_reader_set_y_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg) {}
