#include "app/state.h"
#include "utils/mem.h"

struct AppState {
  void (*open)(void *, app_ctx_t, void *);
  void (*close)(void *);
  void (*destroy)(void *);
  void *private;
};

err_t app_state_create(app_state_t *out,
                       void (*open)(void *, app_ctx_t, void *),
                       void (*close)(void *), void (*destroy)(void *),
                       void *private) {
  app_state_t state = *out = mem_malloc(sizeof(struct AppState));
  *state = (struct AppState){
      .open = open,
      .close = close,
      .destroy = destroy,
      .private = private,
  };

  return 0;
}

void app_state_open(app_state_t astate, app_ctx_t ctx, void *ev_data) {
  if (!astate->open)  {

    return;}
  astate->open(astate->private, ctx, ev_data);
};

void app_state_close(app_state_t astate) {
  if (!astate->close)  {

    return;}

  astate->close(astate->private); };

void app_state_destroy(app_state_t *out) {
  if (!out || !*out) {
    return;
  }

  (*out)->destroy((*out)->private);
  mem_free((*out));
  *out = NULL;
};

void *app_state_get_private(app_state_t astate) { return astate->private; }
