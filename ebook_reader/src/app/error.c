#include "app/core.h"
#include "utils/mem.h"

typedef struct AppError *app_error_t;

struct AppError {
  app_t owner;
  ui_t ui;
};

static void app_error_open(app_module_t, app_ctx_t, void *);
static void app_error_close(app_module_t);
static void app_error_destroy(app_module_t);

err_t app_error_init(app_module_t module, app_t app) {
  app_error_t error = mem_malloc(sizeof(struct AppError));
  *error = (struct AppError){
      .owner = app,
  };
  
  module->open = app_error_open;
  module->close = app_error_close;
  module->destroy = app_error_destroy;

  return 0;
};

static void app_error_open(app_module_t module, app_ctx_t ctx, void *arg) {
  /* app_error_t error = module->private; */
  /* ebk_books_list_t blist; */
  

  }

static void app_error_close(app_module_t module){

};

static void app_error_destroy(app_module_t module){

  };
