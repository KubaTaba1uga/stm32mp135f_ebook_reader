#ifndef EBOOK_READER_GUI
#define EBOOK_READER_GUI
#include "book/book.h"
#include "utils/error.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct ebk_Gui *ebk_gui_t;

enum ebk_GuiInputEventEnum {
  ebk_GuiInputEventEnum_UP,
  ebk_GuiInputEventEnum_DOWN,
  ebk_GuiInputEventEnum_LEFT,
  ebk_GuiInputEventEnum_RIGTH,
  ebk_GuiInputEventEnum_ENTER,
  ebk_GuiInputEventEnum_MENU,
};

extern bool ebk_gui_is_lvgl_init;

ebk_error_t ebk_gui_init(
    ebk_gui_t *out,
    void (*input_callback)(enum ebk_GuiInputEventEnum event, void *data),
    void *input_data);
int ebk_gui_tick(ebk_gui_t gui);
void ebk_gui_destroy(ebk_gui_t *out);
ebk_error_t ebk_gui_show_menu(ebk_gui_t gui, ebk_books_list_t books, uint32_t width, uint32_t height);

#endif // EBOOK_READER_GUI
