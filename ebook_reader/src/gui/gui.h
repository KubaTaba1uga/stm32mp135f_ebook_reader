#ifndef EBOOK_READER_GUI
#define EBOOK_READER_GUI
#include "utils/error.h"
#include <stdbool.h>

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

#endif // EBOOK_READER_GUI
