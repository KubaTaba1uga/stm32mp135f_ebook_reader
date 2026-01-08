/**
   We have two main components, GUI and CORE.

   Core role is to manage consistent state of the device.

   GUI role is to change state of the Core by dispatching user input to the
   Core. For example to go into IN_READER state user needs to select a book
   in menu and click enter. Role of the GUI is to intercept the button press
   as up/down/left/rigth/enter amd dispatch this event to core. Then core role
   is to decide on what screen we are at and do appropriate change in GUI.

   Core and gui both are using the same thread, we decided for such approach
   because the real bottleneck is in screen refresh time and we rarely has a
   need to do anything in a background. The only usecase for doing anything in
   background is init, as far as i see. If this is only real usecase i belive it
   is not suffiecient enough.

   The layout of the repo assumes that any directory in src represent a
   subsystem. Subsystem interface should be an opaque pointer, this convention
   is meant to prevent us from leaking abstractions between subsystems.
   Each subsystem public interface should be in `<subsystem>/<subsystem>.h`
   and it's implementation should be placed in `<subsystem>/<subsystem>.c`.
   If submodule uses multiple src files, put them in
   `<subsystem>/<file_name>.(c|h)`. If submodule src files need to share some
   data with `<subsystem>/<subsystem>.c`, put shared data in
   `<subsystem>/<subsystem>_internal.c`.

   All settings of each subsystem should be placed in `utils/settings.c`,
   remember to declare settings as variables with extern keyword in
   `utils/settings.h` so whole project can be reconfigured without
   recompilation. Only settings.c need to be recompiled.

   Each subsystem can reuse another subsystem, `core` subsystem is propably
   your entrypoint to understanding the app, cause `core` is responsible for
   managing internal state of the app, as well as gui. `core` is splitted into
   logical modules like menu, book reader, settings etc. Each module represent
   user's screen.

   @todo add proper doxygen comment with \@\note \@\warning etc.

 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "core/core.h"
#include "utils/error.h"
#include "utils/log.h"

#define LOG_ERROR(err)                                                         \
  if (!is_err_logged) {                                                        \
    log_error(err);                                                            \
    is_err_logged = true;                                                      \
  }

struct {
  ebk_core_t core;
} ebk_app;

int main(void) {
  bool is_err_logged = false; // We need this var cause cleanup routines can
                              // set different errno along the cleanup way and
                              // we need to ensure that we log only error in the
                              // highest error_X_cleanup routine.

  memset(&ebk_app, 0, sizeof(ebk_app));

  ebk_errno = ebk_core_init(&ebk_app.core);
  EBK_TRY(ebk_errno);

  puts("It's alive");

  ebk_errno = ebk_core_main(ebk_app.core);
  EBK_TRY_CATCH(ebk_errno, error_core_cleanup);

  ebk_core_destroy(&ebk_app.core);
  
  return 0;

error_core_cleanup:
  LOG_ERROR(ebk_errno);
  ebk_core_destroy(&ebk_app.core);
error_out:
  LOG_ERROR(ebk_errno);
  return ebk_errno->code;
}
