/**
   We have two main components, GUI and CORE.

   Core role is to manage consistent state of the device.

   GUI role is to change state of the Core by dispatching user input to the
   Core. For example to go into IN_READER state user needs to select a book
   in menu and click enter. Role of the GUI is to intercept the button press
   as up/down/left/rigth/enter amd dispatch this event to core. Then core role
   is to decide on what screen we are at and do appropriate change in GUI.


   Core

   -----------   init    -------------     open_book()     ---------------
   |  START  | --------> |  IN_MENU  | ------------------> |  IN_READER  |
   -----------           -------------                     ---------------
    ^                              |       open_menu()        |
    |                              +<--------------------------
    | reset()
    |
  --------------
  |  IN_ERROR  |
  --------------


   GUI

   TO-DO

 */

#include <stdio.h>

int main(void) {
  puts("It's alive");
  return 0; 
}
