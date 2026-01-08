/**
   Core assumes we have few possible states of the device.
   Each state is represented by core module, each module is
   responsible for managing whole data gui and libs needed to
   handle the state.

   @todo add error module


   --------------   init   -------------     open_book()     ---------------
   |    INIT    | -------> |  IN_MENU  | ------------------> |  IN_READER  |
   --------------          -------------                     ---------------
                              |   |       open_menu()        |   |
                raise_error() |   +<--------------------------   |
                              |                                  |
   --------------             |           raise_error()          |
   |  IN_ERROR  | <------------------------------------------------
   --------------
      |
      |
      V
   --------------
   |   REBOOT   |
   --------------

  Error can occur in app in two situations. First situation is in core init.
  Once app crash in core's init we are basically done, only thing left to do
  is restart a device.

  But if we crash once the app is initialized, we move to IN_ERROR state
  where error is displayed to user over actual GUI and once user press any
  button device is restarted. Goal in error state is to allow user actually
  read error msg and do not fail silently.

 */
#include "core/book_reader.h"
#include "core/core_internal.h"
#include "core/error.h"
#include "core/menu.h"
#include "display/display.h"
#include "utils/error.h"
#include "utils/mem.h"
#include "utils/settings.h"

struct ebk_Core {
  struct ebk_CoreModule modules[ebk_CoreStateEnum_MAX];
  enum ebk_CoreStateEnum state;
  ebk_display_t display;
  ebk_gui_t gui;
  bool on;
};

ebk_error_t ebk_core_init(ebk_core_t *out) {
  if (!out) {
    ebk_errno = ebk_errnos(EINVAL, "`out` cannot be NULL");
    goto error_out;
  }

  ebk_core_t core = *out = ebk_mem_malloc(sizeof(struct ebk_Core));
  *core = (struct ebk_Core){
      .state = EBK_CORE_STATE_ENUM_INIT,
  };

  ebk_errno = ebk_display_init(&core->display, ebk_settings_display_model);
  EBK_TRY_CATCH(ebk_errno, error_core_cleanup);

  static ebk_error_t (*modules_inits[])(struct ebk_CoreModule *) = {
      [ebk_CoreStateEnum_IN_MENU] = ebk_corem_menu_init,
      [ebk_CoreStateEnum_IN_ERROR] = ebk_corem_error_init,
      [ebk_CoreStateEnum_IN_READER] = ebk_corem_reader_init,
  };

  int inits_status;
  for (inits_status = ebk_CoreStateEnum_IN_MENU;
       inits_status < ebk_CoreStateEnum_MAX; inits_status++) {
    ebk_errno = modules_inits[inits_status](&core->modules[inits_status]);
    EBK_TRY_CATCH(ebk_errno, error_modules_cleanup);
  }

  return 0;

error_modules_cleanup:
  for (; inits_status >= 0; inits_status--) {
    core->modules[inits_status].destroy(&core->modules[inits_status]);
  }
error_core_cleanup:
  ebk_mem_free(*out);
error_out:
  *out = NULL;
  return ebk_errno;
}

ebk_error_t ebk_core_main(ebk_core_t core) {
  if (!core) {
    ebk_errno = ebk_errnos(EINVAL, "`core` cannot be NULL");
    goto error_out;
  }

  return 0;

/* error_core_cleanup: */
/*   ebk_core_sclose(core, core->state); */
/*   return ebk_errno; */

/* error_start_cleanup: */
/*   ebk_start_destroy(&core->start); */
error_out:
  return ebk_errno;
};

void ebk_core_destroy(ebk_core_t *out) {
  if (!out || !*out) {
    return;
  }

  for (int i = ebk_CoreStateEnum_IN_MENU; i < ebk_CoreStateEnum_MAX; i++) {
    (*out)->modules[i].destroy(&(*out)->modules[i]);
  }

  ebk_mem_free(*out);
  *out = NULL;
}
