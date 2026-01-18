#include "app/module.h"
#include "utils/mem.h"

struct AppModule {
  void (*open)(void *, app_ctx_t, void *);
  void (*close)(void *);
  void (*destroy)(void *);
  void *module_data;
};

err_t app_module_create(app_module_t *out,
                        void (*open)(void *, app_ctx_t, void *),
                        void (*close)(void *), void (*destroy)(void *),
                        void *module_data) {
  app_module_t module = *out = mem_malloc(sizeof(struct AppModule));
  *module = (struct AppModule){
      .open = open,
      .close = close,
      .destroy = destroy,
      .module_data = module_data,
  };

  return 0;
}

void app_module_open(app_module_t module, app_ctx_t ctx, void *ev_data) {
  if (!module->open) {

    return;
  }
  module->open(module->module_data, ctx, ev_data);
};

void app_module_close(app_module_t module) {
  if (!module->close) {

    return;
  }

  module->close(module->module_data);
};

void app_module_destroy(app_module_t *out) {
  if (!out || !*out) {
    return;
  }

  (*out)->destroy((*out)->module_data);
  mem_free((*out));
  *out = NULL;
};

void *app_module_get_module_data(app_module_t module) { return module->module_data; }
