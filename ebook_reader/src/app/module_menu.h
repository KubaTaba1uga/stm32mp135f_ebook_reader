#ifndef APP_MODULE_MENU_H
#define APP_MODULE_MENU_H
#include "app/core.h"
#include "app/module.h"
#include "utils/err.h"

err_t app_module_menu_create(app_module_t *, app_t);
void app_module_menu_select_book(void *, app_ctx_t, void *);

#endif // APP_MODULE_MENU_H
