#ifndef UI_H
#define UI_H

#include "book/book.h"
#include "utils/err.h"

typedef struct Ui *ui_t;

enum UiInputEventEnum {
  UiInputEventEnum_UP,
  UiInputEventEnum_DOWN,
  UiInputEventEnum_LEFT,
  UiInputEventEnum_RIGTH,
  UiInputEventEnum_ENTER,
  UiInputEventEnum_MENU,
};

err_t ui_init(ui_t *out,
              void (*callback)(enum UiInputEventEnum event, void *data,
                               void *arg),
              void *data);
int ui_tick(ui_t ui);
void ui_destroy(ui_t *out);

/**
   When system crashes we need to do smallest possible
   cleanup available in ui. Mainly to reset display driver
   to not leave it in wierd state which may cause malfunction
   if not changed for long period of time.
*/
void ui_panic(ui_t ui);

err_t ui_menu_init(ui_t ui, books_list_t blist, int book_i);
void ui_menu_destroy(ui_t ui);

err_t ui_reader_init(ui_t ui, book_t book);
void ui_reader_destroy(ui_t ui);
err_t ui_reader_change_page(ui_t ui, book_t book, int page_i);
err_t ui_reader_settings_init(ui_t ui, const char **fields, int fields_len);
void ui_reader_settings_destroy(ui_t ui);
err_t ui_reader_set_scale_init(ui_t ui, book_t book);
void ui_reader_set_scale_destroy(ui_t ui);




#endif // UI_H
