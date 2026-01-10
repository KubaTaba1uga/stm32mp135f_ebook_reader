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
#include "display/wvs7in5v2b.h"
#include "display/x11.h"
#include "utils/error.h"
#include "utils/mem.h"
#include "utils/settings.h"

ebk_error_t ebk_display_init(ebk_display_t *out,
                             enum ebk_DisplayModelEnum model) {
  puts(__func__);
  ebk_display_t display = *out = ebk_mem_malloc(sizeof(struct ebk_Display));

  switch (model) {
  case ebk_DisplayModelEnum_Wvs7in5V2b:
    ebk_errno = ebk_display_wvs7in5v2b_init(&display->display);
    EBK_TRY(ebk_errno);
    break;
  case ebk_DisplayModelEnum_X11:
    ebk_errno = ebk_display_x11_init(&display->display);
    EBK_TRY(ebk_errno);
    break;
  default:
    ebk_errno =
        ebk_errnof(ENOENT, "Unsupported model: %s", ebk_display_mdump(model));
    EBK_TRY(ebk_errno);
  }

  display->model = model;

  return 0;

error_out:
  ebk_mem_free(*out);
  *out = NULL;
  return ebk_errno;
}

void ebk_display_destroy(ebk_display_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  (*out)->display.destroy(&(*out)->display);
  ebk_mem_free(*out);
  *out = NULL;
}

ebk_error_t ebk_display_show_boot_img(ebk_display_t display) {
  puts(__func__);
  return 0;
}

ebk_error_t ebk_display_show_menu(ebk_display_t display, ebk_gui_t gui) {
  if (!display->display.show_menu) {
    ebk_errno =
        ebk_errnof(EINVAL, "Opening menu is not supported on display: %s",
                   ebk_display_mdump(display->model));
    goto error_out;
  }

  ebk_errno = display->display.show_menu(&display->display, gui);
  EBK_TRY(ebk_errno);

  return 0;

error_out:
  return ebk_errno;
};

const char *ebk_display_mdump(enum ebk_DisplayModelEnum model) {
  static const char *dumps[] = {
      [ebk_DisplayModelEnum_Wvs7in5V2b] = "wvs7in5v2b",
      [ebk_DisplayModelEnum_X11] = "x11",
  };

  if (model < ebk_DisplayModelEnum_Wvs7in5V2b ||
      model > ebk_DisplayModelEnum_X11 || !dumps[model]) {
    return "Unknown";
  }

  return dumps[model];
}
