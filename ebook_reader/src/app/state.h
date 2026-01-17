#ifndef APP_STATE_H
#define APP_STATE_H
#include <lvgl.h>
#include <stdbool.h>

#include "app/core.h"
#include "utils/err.h"

typedef struct AppState *app_state_t;

err_t app_state_create(app_state_t *, void (*)(void *, app_ctx_t, void *),
                       void (*)(void *), void (*)(void *), void *);
void app_state_open(app_state_t, app_ctx_t, void *);
void app_state_close(app_state_t);
void app_state_destroy(app_state_t *);
void * app_state_get_private(app_state_t);

#endif // APP_STATE_H
