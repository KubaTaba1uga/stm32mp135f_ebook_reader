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
#include "core/core.h"
#include "core/book_reader.h"
#include "core/core_internal.h"
#include "core/error.h"
#include "core/menu.h"
#include "display/display.h"
#include "gui/gui.h"
#include "utils/error.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/time.h"

struct ebk_CoreFsmTransition {
  enum ebk_CoreStateEnum next_state;
  /**
     @brief Action that need to be performed upon transition to next_state.

     Action does not return error, because once app is running all errors should
     be reported via ebk_core_error_raise in state module.

   */
  void (*action)(ebk_core_module_t, ebk_core_ctx_t, void *);
};

static const struct ebk_CoreFsmTransition
    fsm_table[ebk_CoreStateEnum_ERROR + 1][ebk_CoreEventEnum_ERROR_RAISED + 1] =
        {
            [ebk_CoreStateEnum_BOOT] =
                {
                    [ebk_CoreEventEnum_BOOT_DONE] =
                        {
                            .next_state = ebk_CoreStateEnum_MENU,
                        },
                    [ebk_CoreEventEnum_ERROR_RAISED] =
                        {
                            .next_state = ebk_CoreStateEnum_ERROR,
                        },
                },
            [ebk_CoreStateEnum_MENU] =
                {
                    [ebk_CoreEventEnum_BTN_ENTER] =
                        {
                            .next_state = ebk_CoreStateEnum_READER,
                        },
                    [ebk_CoreEventEnum_BTN_LEFT] =
                        {
                            .next_state = ebk_CoreStateEnum_MENU,
                            .action = ebk_corem_menu_left,
                        },
                    [ebk_CoreEventEnum_BTN_RIGTH] =
                        {
                            .next_state = ebk_CoreStateEnum_MENU,
                            .action = ebk_corem_menu_rigth,
                        },
                    [ebk_CoreEventEnum_BTN_DOWN] =
                        {
                            .next_state = ebk_CoreStateEnum_MENU,
                            .action = ebk_corem_menu_down,
                        },
                    [ebk_CoreEventEnum_BTN_UP] =
                        {
                            .next_state = ebk_CoreStateEnum_MENU,
                            .action = ebk_corem_menu_up,
                        },
                    [ebk_CoreEventEnum_ERROR_RAISED] =
                        {
                            .next_state = ebk_CoreStateEnum_ERROR,
                        },
                },
            [ebk_CoreStateEnum_READER] =
                {
                    [ebk_CoreEventEnum_BTN_LEFT] =
                        {
                            .next_state = ebk_CoreStateEnum_READER,
                            .action = ebk_corem_reader_prev_page,
                        },
                    [ebk_CoreEventEnum_BTN_RIGTH] =
                        {
                            .next_state = ebk_CoreStateEnum_READER,
                            .action = ebk_corem_reader_next_page,
                        },
                    [ebk_CoreEventEnum_BTN_MENU] =
                        {
                            .next_state = ebk_CoreStateEnum_MENU,
                        },
                    [ebk_CoreEventEnum_ERROR_RAISED] =
                        {
                            .next_state = ebk_CoreStateEnum_ERROR,
                        },
                },
};

struct ebk_CoreFsmEventData {
  enum ebk_CoreEventEnum event;
  void *data;
};

struct ebk_Core {
  struct ebk_CoreModule modules[ebk_CoreStateEnum_ERROR + 1];
  struct ebk_CoreFsmEventData ev_data;
  enum ebk_CoreStateEnum state;
  struct ebk_CoreCtx ctx;
  bool on;
};

static void ebk_core_step(ebk_core_t core);

ebk_error_t ebk_core_init(ebk_core_t *out) {
  if (!out) {
    ebk_errno = ebk_errnos(EINVAL, "`out` cannot be NULL");
    goto error_out;
  }

  ebk_core_t core = *out = ebk_mem_malloc(sizeof(struct ebk_Core));
  *core = (struct ebk_Core){
      .state = ebk_CoreStateEnum_BOOT,
  };

  ebk_errno = ebk_display_init(&core->ctx.display, ebk_settings_display_model);
  EBK_TRY_CATCH(ebk_errno, error_core_cleanup);

  ebk_errno = ebk_display_show_boot_img(core->ctx.display);
  EBK_TRY_CATCH(ebk_errno, error_display_cleanup);

  static ebk_error_t (*modules_inits[])(ebk_core_module_t, ebk_core_t) = {
      [ebk_CoreStateEnum_MENU] = ebk_corem_menu_init,
      [ebk_CoreStateEnum_ERROR] = ebk_corem_error_init,
      [ebk_CoreStateEnum_READER] = ebk_corem_reader_init,
  };

  int inits_status;
  for (inits_status = ebk_CoreStateEnum_MENU;
       inits_status <= ebk_CoreStateEnum_ERROR; inits_status++) {
    ebk_errno = modules_inits[inits_status](&core->modules[inits_status], core);
    EBK_TRY_CATCH(ebk_errno, error_modules_cleanup);
  }

  ebk_errno = ebk_gui_init(&core->ctx.gui);
  EBK_TRY_CATCH(ebk_errno, error_modules_cleanup);

  ebk_core_event_post(core, ebk_CoreEventEnum_BOOT_DONE, NULL);
  ebk_core_step(core);

  // At this moment we are in MENU and menu.open has run.
  // System is fully initialized and ready for processing user events.

  return 0;

error_modules_cleanup:
  for (; inits_status >= ebk_CoreStateEnum_MENU; inits_status--) {
    if (!core->modules[inits_status].destroy) {
      continue;
    }
    core->modules[inits_status].destroy(&core->modules[inits_status]);
  }
error_display_cleanup:
  ebk_display_destroy(&core->ctx.display);
error_core_cleanup:
  ebk_mem_free(*out);
error_out:
  *out = NULL;
  return ebk_errno;
}

/**
   @brief Main event loop of the app.
   @todo Sleep only if ebk_core_step, took less than sleep_ms.
*/
ebk_error_t ebk_core_main(ebk_core_t core) {
  int sleep_ms = 0;

  if (!core) {
    ebk_errno = ebk_errnos(EINVAL, "`core` cannot be NULL");
    goto error_out;
  }

  while (core->on) {
    sleep_ms = ebk_gui_tick(core->ctx.gui);
    if (sleep_ms > 0) {
      ebk_ewrap();
      goto error_out;
    }

    ebk_core_step(core);

    ebk_sleep_ms(sleep_ms);
  }

  return 0;

error_out:
  return ebk_errno;
};

void ebk_core_destroy(ebk_core_t *out) {
  if (!out || !*out) {
    return;
  }

  ebk_gui_destroy(&(*out)->ctx.gui);

  for (int i = ebk_CoreStateEnum_ERROR; i >= ebk_CoreStateEnum_MENU; i--) {
    if (!(*out)->modules[i].destroy) {
      continue;
    }
    (*out)->modules[i].destroy(&(*out)->modules[i]);
  }

  ebk_display_destroy(&(*out)->ctx.display);
  ebk_mem_free(*out);
  *out = NULL;
}

void ebk_core_event_post(ebk_core_t core, enum ebk_CoreEventEnum event,
                         void *data) {

  if (!fsm_table[core->state][event].next_state) {
    log_warn("Unsupported event post: %s:%s -> STATE_NONE",
             ebk_core_sdump(core->state), ebk_core_edump(event));
    goto out;
  }

  if (core->ev_data.event &&                     // Only raising error can
      event != ebk_CoreEventEnum_ERROR_RAISED) { // overwrite existing event.
    log_warn("Overwriting existing event is not supported: %s:%s",
             ebk_core_sdump(core->state), ebk_core_edump(event));
    goto out;
  }

  core->ev_data.event = event;
  core->ev_data.data = data;

out:;
}

const char *ebk_core_sdump(enum ebk_CoreStateEnum state) {
  static char *dumps[] = {
      [ebk_CoreStateEnum_NONE] = "state_none",
      [ebk_CoreStateEnum_MENU] = "state_menu",
      [ebk_CoreStateEnum_BOOT] = "state_boot",
      [ebk_CoreStateEnum_READER] = "state_book_reader",
      [ebk_CoreStateEnum_ERROR] = "state_error",
  };

  if (state < ebk_CoreStateEnum_NONE || state > ebk_CoreStateEnum_ERROR ||
      !dumps[state]) {
    return "Unknown";
  }

  return dumps[state];
};

const char *ebk_core_edump(enum ebk_CoreEventEnum event) {
  static const char *dumps[] = {
      [ebk_CoreEventEnum_NONE] = "ev_none",
      [ebk_CoreEventEnum_BOOT_DONE] = "ev_boot_done",
      [ebk_CoreEventEnum_BTN_ENTER] = "ev_btn_enter",
      [ebk_CoreEventEnum_BTN_MENU] = "ev_btn_menu",
      [ebk_CoreEventEnum_BTN_LEFT] = "ev_btn_left",
      [ebk_CoreEventEnum_BTN_RIGTH] = "ev_btn_right",
      [ebk_CoreEventEnum_BTN_UP] = "ev_btn_up",
      [ebk_CoreEventEnum_BTN_DOWN] = "ev_btn_down",
      [ebk_CoreEventEnum_BOOKS_REFRESHED] = "ev_books_refreshed",
      [ebk_CoreEventEnum_ERROR_RAISED] = "ev_error_raised",
  };

  if (event < ebk_CoreEventEnum_NONE ||
      event > ebk_CoreEventEnum_ERROR_RAISED || !dumps[event]) {
    return "Unknown";
  }

  return dumps[event];
}

void ebk_core_raise_error(ebk_core_t core, ebk_error_t error) {
  ebk_core_event_post(core, ebk_CoreEventEnum_ERROR_RAISED, error);
};

/**
   @brief Move core state according to fsm_table.
   @note We clean core->ev_data before performing step action.
         So every step start with no current event.
*/
static void ebk_core_step(ebk_core_t core) {
  if (!core->ev_data.event) {
    goto out;
  }

  struct ebk_CoreFsmEventData ev_data = core->ev_data;
  memset(&core->ev_data, 0, sizeof(struct ebk_CoreFsmEventData));

  struct ebk_CoreFsmTransition trans = fsm_table[core->state][ev_data.event];
  ebk_core_module_t next_cmodule = &core->modules[trans.next_state];
  ebk_core_module_t cmodule = &core->modules[core->state];

  if (!trans.action && next_cmodule->open) {
    trans.action = next_cmodule->open;
  }

  trans.action(&core->modules[core->state], &core->ctx, ev_data.data);

  if (core->state != trans.next_state && cmodule->close) {
    cmodule->close(cmodule);
  }

  core->state = trans.next_state;

out:;
}
