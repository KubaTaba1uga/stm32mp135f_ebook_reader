#include "app/core.h"
#include "app/module.h"
#include "book/book.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"
#include <stdio.h>

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
static void app_module_reader_page_reopen(app_module_reader_t, app_ctx_t,
                                          enum AppEventEnum, void *);

// Settings API
static void app_module_reader_settings_open(app_module_reader_t, app_ctx_t,
                                            enum AppEventEnum, void *);
static void app_module_reader_settings_select(app_module_reader_t, app_ctx_t,
                                              enum AppEventEnum, void *);
static void app_module_reader_settings_close(app_module_reader_t);

// Set zoom API
static void app_module_reader_set_zoom_open(app_module_reader_t, app_ctx_t,
                                            enum AppEventEnum, void *);
static void app_module_reader_set_zoom_up(app_module_reader_t, app_ctx_t,
                                          enum AppEventEnum, void *);
static void app_module_reader_set_zoom_down(app_module_reader_t, app_ctx_t,
                                            enum AppEventEnum, void *);
static void app_module_reader_set_zoom_close(app_module_reader_t, app_ctx_t,
                                             enum AppEventEnum, void *);

// Set X offset API
static void app_module_reader_set_x_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg);
static void app_module_reader_set_x_off_up(app_module_reader_t, app_ctx_t,
                                           enum AppEventEnum, void *);
static void app_module_reader_set_x_off_down(app_module_reader_t, app_ctx_t,
                                             enum AppEventEnum, void *);
static void app_module_reader_set_x_off_close(app_module_reader_t, app_ctx_t,
                                              enum AppEventEnum, void *);

// Set Y offset API
static void app_module_reader_set_y_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg);
static void app_module_reader_set_y_off_up(app_module_reader_t, app_ctx_t,
                                           enum AppEventEnum, void *);
static void app_module_reader_set_y_off_down(app_module_reader_t, app_ctx_t,
                                             enum AppEventEnum, void *);
static void app_module_reader_set_y_off_close(app_module_reader_t, app_ctx_t,
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
                [AppEventEnum_READER_ZOOM_OPENED] =
                    {
                        .next_state = AppReaderStateEnum_SET_ZOOM,
                        .action = app_module_reader_set_zoom_open,
                    },
                [AppEventEnum_READER_X_OFF_OPENED] =
                    {
                        .next_state = AppReaderStateEnum_SET_X_OFF,
                        .action = app_module_reader_set_x_off_open,
                    },
                [AppEventEnum_READER_Y_OFF_OPENED] =
                    {
                        .next_state = AppReaderStateEnum_SET_Y_OFF,
                        .action = app_module_reader_set_y_off_open,
                    },
                [AppEventEnum_BOOK_SELECTED] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_page_reopen,
                    },
            },
        [AppReaderStateEnum_SET_ZOOM] =
            {
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_set_zoom_close,
                    },
                [AppEventEnum_BTN_UP] =
                    {
                        .next_state = AppReaderStateEnum_SET_ZOOM,
                        .action = app_module_reader_set_zoom_up,
                    },
                [AppEventEnum_BTN_DOWN] =
                    {
                        .next_state = AppReaderStateEnum_SET_ZOOM,
                        .action = app_module_reader_set_zoom_down,
                    },
            },
        [AppReaderStateEnum_SET_X_OFF] =
            {
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_set_x_off_close,
                    },
                [AppEventEnum_BTN_LEFT] =
                    {
                        .next_state = AppReaderStateEnum_SET_X_OFF,
                        .action = app_module_reader_set_x_off_down,
                    },
                [AppEventEnum_BTN_RIGTH] =
                    {
                        .next_state = AppReaderStateEnum_SET_X_OFF,
                        .action = app_module_reader_set_x_off_up,
                    },
            },
        [AppReaderStateEnum_SET_Y_OFF] =
            {
                [AppEventEnum_BTN_ENTER] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_set_y_off_close,
                    },
                [AppEventEnum_BTN_UP] =
                    {
                        .next_state = AppReaderStateEnum_SET_Y_OFF,
                        .action = app_module_reader_set_y_off_down,
                    },
                [AppEventEnum_BTN_DOWN] =
                    {
                        .next_state = AppReaderStateEnum_SET_Y_OFF,
                        .action = app_module_reader_set_y_off_up,
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
  case AppReaderStateEnum_SETTINGS:
    app_module_reader_settings_close(reader);
    break;
  case AppReaderStateEnum_SET_ZOOM:
    ui_reader_set_scale_destroy(reader->ui);
    break;
  case AppReaderStateEnum_SET_X_OFF:
    ui_reader_set_x_off_destroy(reader->ui);
    break;
  case AppReaderStateEnum_SET_Y_OFF:
    ui_reader_set_y_off_destroy(reader->ui);
    break;
  default:;
  }

  reader->state = AppReaderStateEnum_NONE;
  app_module_reader_page_closed(reader);
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

static void app_module_reader_page_reopen(app_module_reader_t reader,
                                          app_ctx_t ctx,
                                          enum AppEventEnum event, void *arg) {
  ui_reader_destroy(ctx->ui);

  app_module_reader_page_open(reader, ctx, event, reader->book);
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

static void app_module_reader_settings_close(app_module_reader_t reader) {
  ui_reader_settings_destroy(reader->ui);
}

static void app_module_reader_set_zoom_open(app_module_reader_t reader,
                                            app_ctx_t ctx,
                                            enum AppEventEnum event, void *arg);
static void app_module_reader_settings_select(app_module_reader_t reader,
                                              app_ctx_t ctx,
                                              enum AppEventEnum event,
                                              void *arg) {
  puts(__func__);
  static enum AppEventEnum next_event[] = {
      AppEventEnum_READER_ZOOM_OPENED,
      AppEventEnum_READER_X_OFF_OPENED,
      AppEventEnum_READER_Y_OFF_OPENED,
      AppEventEnum_BOOK_SELECTED,
  };
  int *id = arg;

  if (*id >= sizeof(fields) / sizeof(char *)) {
    return;
  }

  app_event_post(reader->owner, next_event[*id], NULL);
  ui_reader_settings_destroy(reader->ui);
}

static void app_module_reader_set_zoom_open(app_module_reader_t reader,
                                            app_ctx_t ctx,
                                            enum AppEventEnum event,
                                            void *arg) {
  puts(__func__);
  err_o = ui_reader_set_scale_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_zoom_up(app_module_reader_t reader,
                                          app_ctx_t ctx,
                                          enum AppEventEnum event, void *arg) {
  puts(__func__);
  double scale = book_get_scale(reader->book);
  scale += 0.1;
  book_set_scale(reader->book, scale);

  ui_reader_set_scale_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  err_o = ui_reader_set_scale_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}
static void app_module_reader_set_zoom_down(app_module_reader_t reader,
                                            app_ctx_t ctx,
                                            enum AppEventEnum event,
                                            void *arg) {
  puts(__func__);
  double scale = book_get_scale(reader->book);
  scale -= 0.1;
  book_set_scale(reader->book, scale);

  ui_reader_set_scale_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  err_o = ui_reader_set_scale_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_zoom_close(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg) {
  puts(__func__);

  ui_reader_set_scale_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_x_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg) {
  puts(__func__);
  err_o = ui_reader_set_x_off_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_x_off_up(app_module_reader_t reader,
                                           app_ctx_t ctx,
                                           enum AppEventEnum event, void *arg) {
  puts(__func__);
  double x_off = book_get_x_off(reader->book);
  x_off += 50;
  book_set_x_offset(reader->book, x_off);

  ui_reader_set_x_off_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  err_o = ui_reader_set_x_off_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}
static void app_module_reader_set_x_off_down(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg) {
  puts(__func__);
  double x_off = book_get_x_off(reader->book);
  x_off -= 50;
  book_set_x_offset(reader->book, x_off);

  ui_reader_set_x_off_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  err_o = ui_reader_set_x_off_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_x_off_close(app_module_reader_t reader,
                                              app_ctx_t ctx,
                                              enum AppEventEnum event,
                                              void *arg) {
  puts(__func__);

  ui_reader_set_x_off_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_y_off_open(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg) {
  puts(__func__);
  err_o = ui_reader_set_y_off_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_y_off_up(app_module_reader_t reader,
                                           app_ctx_t ctx,
                                           enum AppEventEnum event, void *arg) {
  puts(__func__);
  double y_off = book_get_y_off(reader->book);
  y_off += 50;
  book_set_y_offset(reader->book, y_off);

  ui_reader_set_y_off_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  err_o = ui_reader_set_y_off_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}
static void app_module_reader_set_y_off_down(app_module_reader_t reader,
                                             app_ctx_t ctx,
                                             enum AppEventEnum event,
                                             void *arg) {
  puts(__func__);
  double y_off = book_get_y_off(reader->book);
  y_off -= 50;
  book_set_y_offset(reader->book, y_off);

  ui_reader_set_y_off_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  err_o = ui_reader_set_y_off_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}

static void app_module_reader_set_y_off_close(app_module_reader_t reader,
                                              app_ctx_t ctx,
                                              enum AppEventEnum event,
                                              void *arg) {
  puts(__func__);

  ui_reader_set_y_off_destroy(ctx->ui);
  ui_reader_destroy(reader->ui);

  err_o = ui_reader_init(ctx->ui, reader->book);
  ERR_TRY(err_o);

  return;

error_out:
  app_raise_error(reader->owner, err_o);
}
