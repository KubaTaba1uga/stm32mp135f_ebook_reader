#ifndef EBOOK_READER_APP_H
#define EBOOK_READER_APP_H

#include "utils/err.h"

typedef struct App *app_t;

err_t app_init(app_t *out);
err_t app_main(app_t out);
void app_destroy(app_t *out);

#endif // EBOOK_READER_APP_H
