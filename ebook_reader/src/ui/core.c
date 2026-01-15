#include <error.h>
#include <stdio.h>

#include "ui/display.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/settings.h"

enum UiInputEventEnum {
  UiInputEventEnum_UP,
  UiInputEventEnum_DOWN,
  UiInputEventEnum_LEFT,
  UiInputEventEnum_RIGTH,
  UiInputEventEnum_ENTER,
  UiInputEventEnum_MENU,
};

struct Ui {
  struct {
    void (*callback)(enum UiInputEventEnum event, void *data, void *arg);
    void *data;
  } inputh;

  enum UiDisplayEnum current_display;
  struct UiDisplay display;
};

err_t ui_init(ui_t *out) {
  static err_t (*displays_inits[])(ui_display_t, ui_t) = {
      [UiDisplayEnum_X11] = ui_display_x11_init,
  };
  ui_t ui = *out = mem_malloc(sizeof(struct Ui));
  *ui = (struct Ui){0};

  lv_init();

  /**
     @todo This should be in display_init
  */
  bool is_display_found = false;
  for (int i = UiDisplayEnum_X11; i < sizeof(displays_inits) / sizeof(void *);
       i++) {
    if (!displays_inits[i]) {
      continue;
    }

    err_errno = displays_inits[i](&ui->display, ui);
    if (!err_errno) {
      ui->current_display = i;
      is_display_found = true;
      break;
    }

    log_error(err_errno);
  }

  if (!is_display_found) {
    err_errno = err_errnos(ENODEV, "Cannot initialize display");
    goto error_out;
  }

  /**
     @todo This should be in display render boot img.
  */
  FILE *boot_screen_fd = fopen(settings_boot_screen_path, "r");
  if (!boot_screen_fd) {
    err_errno = err_errnof(ENOENT, "There is no file like %s",
                           settings_boot_screen_path);
    goto error_display_cleanup;
  }

  unsigned char *img_buf = mem_malloc(48000);
  const size_t ret_code = fread(img_buf, 1, 48000, boot_screen_fd);
  if (ret_code != 48000) {
    err_errno =
        err_errnof(ENOENT, "Cannot read file %s", settings_boot_screen_path);
    goto error_boot_screen_cleanup;
  }
  fclose(boot_screen_fd);
  
  err_errno =
      ui->display.render(&ui->display, (unsigned char *)img_buf);
  ERR_TRY_CATCH(err_errno, error_boot_screen_cleanup);
  
  return 0;

error_boot_screen_cleanup:
  fclose(boot_screen_fd);
error_display_cleanup:
  ui->display.destroy(&ui->display);
error_out:
  mem_free(*out);
  *out = NULL;
  return err_errno;
}
