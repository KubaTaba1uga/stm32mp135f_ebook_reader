/**
   Core assumes we have few possible states of the device.
   Each state is represented by core module, each module is
   responsible for managing whole data gui and libs needed to
   handle the state.

   Any module can put core in error state, then error is
   displayed on the screen for 20s and the device reboots.

   @todo add error module


   --------------   init   -------------     open_book()     ---------------
   |  STARTING  | -------> |  IN_MENU  | ------------------> |  IN_READER  |
   --------------          -------------                     ---------------
                                  |       open_menu()        |
                                  +<--------------------------
  --------------
  |  IN_ERROR  |
  --------------


+--------+   init/probe ok    +--------+   clear/write    +--------+
|  NEW   | -----------------> | READY  | ---------------> | ACTIVE |
+--------+                    +--------+                  +--------+
   |  init/probe fail              ^   | power_off done         |
   v                               |   +------------------------+
+--------+   destroy               |
| ERROR  | <-----------------------+
+--------+

 */
#include "core/book_reader.h"
#include "core/core_internal.h"
#include "core/menu.h"
#include "core/start.h"
#include "utils/error.h"
#include "utils/log.h"
#include "utils/mem.h"

ebk_error_t ebk_core_init(ebk_core_t *out) {
  if (!out) {
    ebk_errno = ebk_errnos(EINVAL, "`out` cannot be NULL");
    goto error_out;
  }

  *out = ebk_mem_malloc(sizeof(struct ebk_Core));
  **out = (struct ebk_Core){
      .state = EBK_CORE_STATE_ENUM_INIT,
  };

  ebk_errno = ebk_core_sinit(*out, ebk_CoreStateEnum_STARTING);
  EBK_TRY_CATCH(ebk_errno, error_core_cleanup);
  
  ebk_errno = ebk_core_sopen(*out, ebk_CoreStateEnum_STARTING,
                            &(struct ebk_StartOpenConfig){
                                .core = *out,
                            });
  EBK_TRY_CATCH(ebk_errno, error_start_cleanup);

  return 0;

error_start_cleanup:
  ebk_start_destroy(&(*out)->start);
error_core_cleanup:
  ebk_mem_free(*out);
error_out:
  *out = NULL;  
  return ebk_errno;
}

ebk_error_t ebk_core_main(ebk_core_t core){
  if (!core) {
    ebk_errno = ebk_errnos(EINVAL, "`core` cannot be NULL");
    goto error_out;
  }

  ebk_errno = ebk_core_sinit(core, ebk_CoreStateEnum_STARTING);
  EBK_TRY(ebk_errno);
  
  ebk_errno = ebk_core_sopen(core, ebk_CoreStateEnum_STARTING,
                            &(struct ebk_StartOpenConfig){
                                .core = core,
                            });
  EBK_TRY_CATCH(ebk_errno, error_start_cleanup);

  core->on = true;
  while(core->on){
    ebk_errno = ebk_gui_tick(core->gui);
    EBK_TRY_CATCH(ebk_errno, error_core_cleanup);
  }

  return 0;

error_core_cleanup:
  ebk_core_sclose(core, core->state);
  return ebk_errno;
  
error_start_cleanup:
  ebk_start_destroy(&core->start);
error_out:
  return ebk_errno;  
};

ebk_error_t ebk_core_sinit(ebk_core_t core, enum ebk_CoreStateEnum state) {
  switch (state) {
  case ebk_CoreStateEnum_STARTING:
    ebk_errno = ebk_start_init(&core->start);
    EBK_TRY(ebk_errno);
    break;
  case ebk_CoreStateEnum_IN_MENU:
    ebk_errno = ebk_menu_init(&core->menu);
    EBK_TRY(ebk_errno);
    break;
  case ebk_CoreStateEnum_IN_READER:
    ebk_errno = ebk_reader_init(&core->reader);
    EBK_TRY(ebk_errno);
    break;
  default:
    ebk_errno = ebk_errnof(EINVAL, "State does not support init: %d", state);
    EBK_TRY(ebk_errno);
  }

  return 0;

error_out:
  return ebk_errno;  
};

ebk_error_t ebk_core_sopen(ebk_core_t core, enum ebk_CoreStateEnum state,
                           void *config) {
  log_debug("%s -> %s", ebk_core_sdump(core->state), ebk_core_sdump(state));

  switch (state) {
  case ebk_CoreStateEnum_STARTING:
    ebk_errno = ebk_start_open(&core->start, config);
    EBK_TRY(ebk_errno);
    break;
  case ebk_CoreStateEnum_IN_MENU:
    ebk_errno = ebk_menu_open(&core->menu, config);
    EBK_TRY(ebk_errno);
    break;
  case ebk_CoreStateEnum_IN_READER:
    ebk_errno = ebk_reader_open(&core->reader, config);
    EBK_TRY(ebk_errno);
    break;
  default:
    ebk_errno = ebk_errnof(EINVAL, "State does not support open: %d", state);
    EBK_TRY(ebk_errno);
  }

  ebk_core_sclose(core, core->state);
  core->state = state;

  return 0;

error_out:
  return ebk_errno;
};

void ebk_core_sclose(ebk_core_t core, enum ebk_CoreStateEnum state) {
  switch (state) {
  case ebk_CoreStateEnum_STARTING:
    ebk_start_close(&core->start);
    break;
  case ebk_CoreStateEnum_IN_MENU:
    ebk_menu_close(&core->menu);
    break;
  case ebk_CoreStateEnum_IN_READER:
    ebk_reader_close(&core->reader);
    break;

  default:
    if (state != EBK_CORE_STATE_ENUM_INIT) { // init state does not need close
      log_error(
          ebk_errnof(EINVAL, "State does not support close: %d", core->state));
    }
  }

  core->state = EBK_CORE_STATE_ENUM_INIT;
};

void ebk_core_destroy(ebk_core_t *out) {
  if (!out || !*out) {
    return;
  }

  ebk_core_sclose(*out, (*out)->state);
  ebk_reader_destroy(&(*out)->reader);
  ebk_menu_destroy(&(*out)->menu);
  ebk_start_destroy(&(*out)->start);
  ebk_mem_free(*out);
  *out = NULL;
}

