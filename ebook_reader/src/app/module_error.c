#include <stdio.h>

#include "app/core.h"
#include "app/module.h"
#include "utils/log.h"
#include "utils/mem.h"


typedef struct AppError *app_module_error_t;

struct AppError {
  app_t owner;
  ui_t ui;
};

static void app_module_error_open(void *, app_ctx_t, void *);
static void app_module_error_close(void *);
static void app_module_error_destroy(void *);

err_t app_module_error_create(app_module_t *out, app_t app) {
  app_module_error_t error = mem_malloc(sizeof(struct AppError));
  *error = (struct AppError){
      .owner = app,
  };

  err_o = app_module_create(out, app_module_error_open, app_module_error_close,
                           app_module_error_destroy, error);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

static void app_module_error_open(void *module, app_ctx_t ctx, void *arg) {
  log_error(arg);
}

static void app_module_error_close(void *module) {

};

static void app_module_error_destroy(void *module) { mem_free(module); };
