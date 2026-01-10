/**
   Because we need to support multiple displays we need some kind of
   abstractions over display models. One display can have different
   resolution, or different color pallete, gola of this abstraction
   is to allow displaying our gui on various different screens.

 */
#include <display_driver.h>
#include <stdio.h>

#include "display/display.h"
#include "display/display_internal.h"
#include "utils/error.h"


ebk_error_t ebk_display_init(ebk_display_t *out,
                             enum dd_DisplayDriverEnum model) {
  puts(__func__);
  return 0;
}

void ebk_display_destroy(ebk_display_t *out) { puts(__func__); }

ebk_error_t ebk_display_show_boot_img(ebk_display_t display) {
  puts(__func__);
  return 0;
}
