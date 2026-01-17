#include <stdio.h>

#include "app/core.h"
#include "app/state.h"
#include "utils/log.h"
#include "utils/mem.h"

typedef struct AppReader *app_state_reader_t;

struct AppReader {
  app_t owner;
  ui_t ui;
};

static void app_state_reader_open(void *, app_ctx_t, void *);
static void app_state_reader_close(void *);
static void app_state_reader_destroy(void *);

err_t app_state_reader_create(app_state_t *out, app_t app) {
  app_state_reader_t reader = mem_malloc(sizeof(struct AppReader));
  *reader = (struct AppReader){
      .owner = app,
  };

  err_o = app_state_create(out, app_state_reader_open, app_state_reader_close,
                           app_state_reader_destroy, reader);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

static void app_state_reader_open(void *astate, app_ctx_t ctx, void *arg) {}

static void app_state_reader_close(void *astate) {

};

static void app_state_reader_destroy(void *astate) { mem_free(astate); };
