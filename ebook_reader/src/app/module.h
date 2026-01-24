#ifndef APP_MODULE_H
#define APP_MODULE_H

#include <lvgl.h>
#include <stdbool.h>

#include "app/core.h"
#include "utils/err.h"

/**
   @brief App module describes application behaviour in particular state.
*/
typedef struct AppModule *app_module_t;

struct AppModule {
  void (*open)(void *, app_ctx_t, void *);
  void (*close)(void *);
  void (*destroy)(void *);
  void *module_data;
};

err_t app_module_init(app_module_t, app_t, enum AppStateEnum);
void app_module_open(app_module_t, app_ctx_t, void *);
void app_module_close(app_module_t);
void app_module_destroy(app_module_t);
void app_modules_destroy(app_module_t, int );

err_t app_module_menu_init(app_module_t, app_t);
void app_module_menu_select_book(app_module_t, app_ctx_t, void *);
err_t app_module_reader_init(app_module_t, app_t);
err_t app_module_error_init(app_module_t, app_t);

#endif // APP_MODULE_H
