#include <stdio.h>

#include "app/core.h"
#include "app/state.h"
#include "utils/log.h"
#include "utils/mem.h"


typedef struct AppError *app_state_error_t;

struct AppError {
  app_t owner;
  ui_t ui;
};

static void app_state_error_open(void *, app_ctx_t, void *);
static void app_state_error_close(void *);
static void app_state_error_destroy(void *);

err_t app_state_error_create(app_state_t *out, app_t app) {
  app_state_error_t error = mem_malloc(sizeof(struct AppError));
  *error = (struct AppError){
      .owner = app,
  };

  err_o = app_state_create(out, app_state_error_open, app_state_error_close,
                           app_state_error_destroy, error);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

static void app_state_error_open(void *astate, app_ctx_t ctx, void *arg) {
  log_error(arg);
}

static void app_state_error_close(void *astate) {

};

static void app_state_error_destroy(void *astate) { mem_free(astate); };
