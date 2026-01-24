#include <stdio.h>

#include "app/core.h"
#include "app/module.h"
#include "utils/log.h"
#include "utils/mem.h"

typedef struct AppReader *app_module_reader_t;

struct AppReader {
  app_t owner;
  ui_t ui;
};

static void app_module_reader_open(void *, app_ctx_t, void *);
static void app_module_reader_close(void *);
static void app_module_reader_destroy(void *);

err_t app_module_reader_init(app_module_t out, app_t app) {
  app_module_reader_t reader = mem_malloc(sizeof(struct AppReader));
  *reader = (struct AppReader){
      .owner = app,
  };

  *out = (struct AppModule){
      .open = app_module_reader_open,
      .close = app_module_reader_close,
      .destroy = app_module_reader_destroy,
      .module_data = reader,
  };

  return 0;
};

static void app_module_reader_open(void *module, app_ctx_t ctx, void *arg) {}

static void app_module_reader_close(void *module) {

};

static void app_module_reader_destroy(void *module) { mem_free(module); };
