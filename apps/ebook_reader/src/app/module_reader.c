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
  AppReaderStateEnum_MAX,
};

struct AppReader {
  enum AppReaderStateEnum state;
  int x_offset;
  int y_offset;
  book_t book;  
  app_t owner;
  ui_t ui;
};

struct AppReaderFsmTransition {
  enum AppReaderStateEnum next_state;
  void (*action)(app_module_reader_t, app_ctx_t, enum AppEventEnum, void *);
};

static const char *app_reader_state_dump(enum AppReaderStateEnum);
static void app_module_reader_open_page(app_module_reader_t, app_ctx_t,
                                        enum AppEventEnum, void *);
static void app_module_reader_step(void *, app_ctx_t, enum AppEventEnum,
                                   void *);
static void app_module_reader_close_page(app_module_reader_t);
static void app_module_reader_close(void *);
static void app_module_reader_destroy(void *);

struct AppReaderFsmTransition
    reader_fsm_table[AppReaderStateEnum_MAX][AppEventEnum_MAX] = {
        [AppReaderStateEnum_NONE] =
            {
                [AppEventEnum_BOOK_SELECTED] =
                    {
                        .next_state = AppReaderStateEnum_PAGE,
                        .action = app_module_reader_open_page,
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

static void app_module_reader_step(void *module, app_ctx_t ctx,
                                   enum AppEventEnum event, void *arg) {
  app_module_reader_t reader = module;
  struct AppReaderFsmTransition trans;

  trans = reader_fsm_table[reader->state][event];

  if (!trans.action) {
    log_debug("Triggered transaction without action: event=%s, state=%s",
              app_event_dump(event), app_reader_state_dump(reader->state));
    goto out;
  }

  log_debug("%s -> %s, event=%s", app_reader_state_dump(reader->state),
            app_reader_state_dump(trans.next_state), app_event_dump(event));

  trans.action(reader, ctx, event, arg);
  reader->state = trans.next_state;

out:;
}

static void app_module_reader_close(void *module) {
  app_module_reader_t reader = module;

  switch (reader->state) {
  case AppReaderStateEnum_PAGE:
    app_module_reader_close_page(reader);
    break;
  default:;
  }

  reader->state = AppReaderStateEnum_NONE;
  ui_reader_destroy(reader->ui);

  reader->ui = NULL;
};

static void app_module_reader_destroy(void *module) { mem_free(module); };

static void app_module_reader_open_page(app_module_reader_t reader,
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

static void app_module_reader_close_page(app_module_reader_t reader) {
  ui_reader_destroy(reader->ui);
  book_destroy(&reader->book);
};

static const char *app_reader_state_dump(enum AppReaderStateEnum state) {
  static char *dumps[] = {
      [AppReaderStateEnum_PAGE] = "state_page",
  };

  if (state <= AppReaderStateEnum_NONE || state >= AppReaderStateEnum_MAX ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};
