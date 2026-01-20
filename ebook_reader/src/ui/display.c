#include <assert.h>
#include <lvgl.h>
#include <stdint.h>

#include "display/lv_display.h"
#include "indev/lv_indev.h"
#include "ui/display.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/settings.h"

err_t ui_display_init(ui_display_t out, ui_t ui, enum DisplayModelEnum model) {
  err_t (*creates[])(ui_display_t, ui_t) = {
      [DisplayModelEnum_X11] = ui_display_x11_init,
      [DisplayModelEnum_WVS7IN5V2B] = ui_display_dd_init,
      [DisplayModelEnum_WVS7IN5V2] = ui_display_dd_init,
  };

  err_o = creates[model](out, ui);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}

err_t ui_display_render(ui_display_t display, unsigned char *buf,
                        uint32_t buf_len) {
  assert(display->render != NULL); // Wevery display need render

  err_o = display->render(display->owner, buf, buf_len);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}

void ui_display_destroy(ui_display_t display) {
  if (display->destroy) {
    display->destroy(display->owner);
  }
  if (display->lv_ingroup) {
    lv_group_delete(display->lv_ingroup);
  }
  
  
}

void ui_display_panic(ui_display_t display) {
  if (display->panic) {
    display->panic(display->owner);
  }
}

lv_group_t *ui_display_get_input_group(ui_display_t display) {
  if (display->lv_ingroup) {
    return display->lv_ingroup;
  }

  return lv_indev_get_group(display->lv_indv);
}
