#ifndef APP_STATE_MENU_H
#define APP_STATE_MENU_H
#include "app/core.h"
#include "app/state.h"
#include "utils/err.h"

err_t app_state_menu_create(app_state_t *, app_t);
void app_state_menu_select_book(app_state_t, app_ctx_t, void *);

#endif // APP_STATE_MENU_H
