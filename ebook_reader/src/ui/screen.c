#include "ui/screen.h"
#include <stdio.h>
#include <string.h>

void ui_screen_destroy(ui_screen_t out) {
  puts(__func__);  
  if (!out) {
    return;
  }

  if (out->destroy) {
puts("Destroy");    
    out->destroy(out->screen_data);
    
  }else {    puts("No destroy");
}

  memset(out, 0, sizeof(struct UiScreen));
};
