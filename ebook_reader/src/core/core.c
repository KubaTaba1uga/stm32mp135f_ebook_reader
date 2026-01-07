/**
   Core assumes we have few possible states of the device.

   -----------   init   -------------     open_book()     ---------------
   |  START  | -------> |  IN_MENU  | ------------------> |  IN_READER  |
   -----------          -------------                     ---------------
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
#include "core.h"


enum cbk_CoreStateEnum {
  cbk_CoreStateEnum_START = 0,
  cbk_CoreStateEnum_IN_MENU,
  cbk_CoreStateEnum_IN_READER,
  cbk_CoreStateEnum_IN_ERROR,
};

struct cbk_Core {
  enum cbk_CoreStateEnum state;
};

ebk_error_t ebk_core_init(ebk_core_t *out) {
  if (!out) {
    /* ebk_errno */
  }

  return 0;
}


