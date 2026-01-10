#include "gui/gui.h"
#include "utils/error.h"
#include <stdio.h>


ebk_error_t ebk_gui_init(ebk_gui_t *out) {
  
  puts(__func__);
  return 0;
}

int ebk_gui_tick(ebk_gui_t gui) {
  puts(__func__);
  return 1000;
}

void ebk_gui_destroy(ebk_gui_t *out) { puts(__func__); }
