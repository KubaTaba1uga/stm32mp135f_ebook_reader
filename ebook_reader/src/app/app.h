#ifndef APP_H
#define APP_H
#include "utils/err.h"

typedef struct App *app_t;

err_t app_init(app_t *out);
err_t app_main(app_t app);
void app_destroy(app_t *out);
void app_panic(app_t app);

#endif // APP_H
