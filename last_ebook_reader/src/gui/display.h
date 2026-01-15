#ifndef EBOOK_READER_GUI_DISPLAY_H
#define EBOOK_READER_GUI_DISPLAY_H
#include <lvgl.h>
#include <stdbool.h>
#include <stdint.h>

#include "utils/error.h"
#include "gui/gui.h"

typedef struct ebk_GuiDisplay *ebk_gui_display_t;


enum ebk_GuiDisplayEnum {
  ebk_GuiDisplayEnum_X11,
  ebk_GuiDisplayEnum_WVS7IN5V2B,
};

struct ebk_GuiDisplay {
  lv_display_t *(*get_lv_display)(ebk_gui_display_t);
  ebk_error_t (*render)(ebk_gui_display_t, unsigned char *);
  int (*get_render_size)(ebk_gui_display_t);
  int (*get_render_x)(ebk_gui_display_t);
  int (*get_render_y)(ebk_gui_display_t);
  void (*destroy)(ebk_gui_display_t);
  void *private;
};

ebk_error_t ebk_gui_display_x11_init(ebk_gui_display_t, ebk_gui_t);

#endif // EBOOK_READER_GUI_DISPLAY_H
