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

err_t app_module_create(app_module_t *, void (*)(void *, app_ctx_t, void *),
                       void (*)(void *), void (*)(void *), void *);
void app_module_open(app_module_t, app_ctx_t, void *);
void app_module_close(app_module_t);
void app_module_destroy(app_module_t *);

/**
   @brief We need get module_data so each module can be compatible with fsm
          transaction.
 */
void * app_module_get_module_data(app_module_t);

#endif // APP_MODULE_H
