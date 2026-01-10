#ifndef EBOOK_READER_DISPLAY_H
#define EBOOK_READER_DISPLAY_H
#include <display_driver.h>

#include "gui/gui.h"
#include "utils/error.h"
#include "utils/settings.h"

typedef struct ebk_Display *ebk_display_t;

ebk_error_t ebk_display_init(ebk_display_t *, enum ebk_DisplayModelEnum);
ebk_error_t ebk_display_show_boot_img(ebk_display_t);

/**
   @brief Show menu on the screen.

   Display is responsible for hardware layer, for UX layer responsible is gui.
   To show menu, display feed gui with callbacks and data required to display
   menu on the screen. Goal of this separation is to provide generic framework
   for all UX.

   Because LVGL provide also display driver for X11, we can use LVGL directly
   in display. 

*/
ebk_error_t ebk_display_show_menu(ebk_display_t, ebk_gui_t);
void ebk_display_destroy(ebk_display_t *);

#endif // EBOOK_READER_DISPLAY_H
