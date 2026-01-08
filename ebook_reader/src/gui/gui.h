#ifndef EBOOK_READER_GUI
#define EBOOK_READER_GUI

#include "utils/error.h"
typedef struct ebk_Gui *ebk_gui_t;

ebk_error_t ebk_gui_init(ebk_gui_t *out);
ebk_error_t ebk_gui_display_boot_img(ebk_gui_t gui);
ebk_error_t ebk_gui_display_boot_img_text(ebk_gui_t gui, const char *txt);
ebk_error_t ebk_gui_tick(ebk_gui_t gui);
void ebk_gui_destroy(ebk_gui_t *out);

#endif // EBOOK_READER_GUI
