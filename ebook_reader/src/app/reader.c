#include "app/core.h"
#include "utils/mem.h"

typedef struct AppReader *app_reader_t;

struct AppReader {
  app_t owner;
  ui_t ui;
};

static void app_reader_open(app_module_t, app_ctx_t, void *);
static void app_reader_close(app_module_t);
static void app_reader_destroy(app_module_t);

err_t app_reader_init(app_module_t module, app_t app) {
  app_reader_t reader = mem_malloc(sizeof(struct AppReader));
  *reader = (struct AppReader){
      .owner = app,
  };

  module->open = app_reader_open;
  module->close = app_reader_close;
  module->destroy = app_reader_destroy;

  return 0;
};

static void app_reader_open(app_module_t module, app_ctx_t ctx, void *arg) {
  /* app_reader_t reader = module->private; */
  /* ebk_books_list_t blist; */
}

static void app_reader_close(app_module_t module) {

};

static void app_reader_destroy(app_module_t module) {

};
