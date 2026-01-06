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

enum cbk_CoreStateEnum {
  cbk_CoreStateEnum_START,
  cbk_CoreStateEnum_IN_MENU,
  cbk_CoreStateEnum_IN_READER,
  cbk_CoreStateEnum_IN_ERROR,
};

struct cbk_Core {
  enum cbk_CoreStateEnum state;
  
};


