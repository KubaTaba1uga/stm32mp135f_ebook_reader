#include <lvgl.h>
#include <stdint.h>

#include "ui/display.h"
#include "ui/display_wvs7in5v2.h"
#include "ui/display_wvs7in5v2b.h"
#include "ui/display_x11.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/settings.h"

struct UiDisplay {
  err_t (*render_create)(void *, unsigned char *, uint32_t);
  void (*render_destroy)(void *);
  void (*destroy)(void *);
  void (*panic)(void *);
  lv_display_t *lv_obj;
  void *private;
  ui_t owner;
};

err_t ui_display_create(ui_display_t *out, lv_display_t *lv_obj, ui_t ui,
                        err_t (*render_create)(void *, unsigned char *,
                                               uint32_t),
                        void (*render_destroy)(void *), void (*destroy)(void *),
                        void (*panic)(void *), void *data) {
  ui_display_t display = *out = mem_malloc(sizeof(struct UiDisplay));
  display->render_destroy = render_destroy;
  display->render_create = render_create;
  display->destroy = destroy;
  display->lv_obj = lv_obj;
  display->private = data;
  display->panic = panic;
  display->owner = ui;

  return 0;
}

err_t ui_display_render_create(ui_display_t display, unsigned char *buf,
                               uint32_t buf_len) {
  if (!display->render_create) {
    return 0;
  };

  err_o = display->render_create(display->private, buf, buf_len);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
};

void ui_display_render_destroy(ui_display_t display) {
  if (!display->render_destroy) {
    return;
  };

  display->render_destroy(display->private);
}
void ui_display_panic(ui_display_t display) {
  if (!display->panic) {
    return;
  };

  display->panic(display->private);
}

void ui_display_destroy(ui_display_t *out) {
  if (!out || !*out) {
    return;
  }

  (*out)->destroy((*out)->private);
  /* lv_display_delete((*out)->lv_obj); // This line causes crash in exit
   * handler  */
  mem_free(*out);
  *out = NULL;
};

ui_t ui_display_get_ui(ui_display_t display) { return display->owner; }

lv_display_t *ui_display_get_lv_obj(ui_display_t display) {
  return display->lv_obj;
}

err_t ui_display_supported_create(ui_display_t *out, ui_t ui,
                                  enum DisplayModelEnum model) {
  err_t (*creates[])(ui_display_t *, ui_t) = {
      [DisplayModelEnum_X11] = ui_display_x11_create,
      [DisplayModelEnum_WVS7IN5V2B] = ui_display_wvs7in5v2b_create,
      [DisplayModelEnum_WVS7IN5V2] = ui_display_wvs7in5v2_create,
  };

  err_o = creates[model](out, ui);
  ERR_TRY(err_o);

  return 0;

error_out:
  return err_o;
}

err_t ui_display_show_boot_img(ui_display_t display, const char *img_path) {
  FILE *boot_screen_fd = fopen(img_path, "r");
  if (!boot_screen_fd) {
    err_o = err_errnof(ENOENT, "There is no file like %s",
                       settings_boot_screen_path);
    goto error_out;
  }

  unsigned char *img_buf = mem_malloc(48000+1);
  const size_t ret_code = fread(img_buf, 1, 48000, boot_screen_fd);
  if (ret_code != 48000) {
    err_o = err_errnof(ENOENT, "Cannot read file %s", img_path);
    goto error_boot_screen_cleanup;
  }
  fclose(boot_screen_fd);

  err_o = ui_display_render_create(display, img_buf, 48000);
  ERR_TRY_CATCH(err_o, error_boot_screen_2_cleanup);

  return 0;

error_boot_screen_cleanup:
  fclose(boot_screen_fd);
error_boot_screen_2_cleanup:
  mem_free(img_buf);
error_out:
  return err_o;
};
