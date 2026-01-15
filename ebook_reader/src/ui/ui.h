#ifndef UI_H
#define UI_H

#include "book/book.h"
#include "utils/err.h"

typedef struct Ui *ui_t;

err_t ui_init(ui_t *out);

int ui_tick(ui_t ui);
err_t ui_menu_create(ui_t ui, books_list_t blist, int book_i);
void ui_destroy(ui_t *out);

#endif // UI_H
