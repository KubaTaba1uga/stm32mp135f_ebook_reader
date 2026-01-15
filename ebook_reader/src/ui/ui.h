#ifndef EBOOK_READER_UI_CORE_H
#define EBOOK_READER_UI_CORE_H

#include "utils/err.h"

typedef struct Ui *ui_t;

err_t ui_init(ui_t *out);
void ui_destroy(ui_t *out);

#endif // EBOOK_READER_UI_CORE_H
