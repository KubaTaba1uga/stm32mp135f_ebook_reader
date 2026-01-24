#include "app/module.h"
#include "utils/err.h"
#include "app/core.h"

err_t app_module_init(app_module_t module, app_t app, enum AppStateEnum state) {
  err_t (*inits[])(app_module_t, app_t) = {
      [AppStateEnum_MENU] = app_module_menu_init,
      [AppStateEnum_READER] = app_module_reader_init,
      [AppStateEnum_ERROR] = app_module_error_init,
  };

  err_o = inits[state](module, app);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
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

void app_module_destroy(app_module_t out) {
  if (!out) {
    return;
  }

  out->destroy(out->module_data);
};

void *app_module_get_module_data(app_module_t module) {
  return module->module_data;
}

void app_modules_destroy(app_module_t modules, int modules_len) {
  while (--modules_len) {
    app_module_destroy(&modules[modules_len]);
  }
}
