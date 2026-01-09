#ifndef EBOOK_READER_CORE_INTERNAL_H
#define EBOOK_READER_CORE_INTERNAL_H
#include <stdbool.h>

#include "core/core.h"
#include "display/display.h"
#include "gui/gui.h"
#include "utils/error.h"

typedef struct ebk_Core *ebk_core_t;
typedef struct ebk_CoreCtx *ebk_core_ctx_t;
typedef struct ebk_CoreModule *ebk_core_module_t;

#define EBK_CORE_STATE_ENUM_INIT -1

enum ebk_CoreStateEnum {
  ebk_CoreStateEnum_NONE = 0,
  ebk_CoreStateEnum_BOOT,
  ebk_CoreStateEnum_MENU,
  ebk_CoreStateEnum_READER,
  // Add more states here
  ebk_CoreStateEnum_ERROR,
};

struct ebk_CoreModule {
  void (*open)(ebk_core_module_t, ebk_core_ctx_t, void *);
  void (*close)(ebk_core_module_t);
  void (*destroy)(ebk_core_module_t);
  void *private;
};

struct ebk_CoreCtx {
  ebk_gui_t gui;
  ebk_display_t display;
};

/**
   @brief Events occuring in the system.
 */
enum ebk_CoreEventEnum {
  ebk_CoreEventEnum_NONE = 0,
  /**
   Once device finish boot we emmit BOOT_DONE.
   This is first event which occur in the system.
   */
  ebk_CoreEventEnum_BOOT_DONE,
  /**
   Once enter button is pressed we emmit BTN_ENTER.
   */
  ebk_CoreEventEnum_BTN_ENTER,
  /**
   Once menu button is pressed we emmit BTN_MENU.
   */
  ebk_CoreEventEnum_BTN_MENU,
  /**
   Once left button is pressed we emmit BTN_LEFT.
   */
  ebk_CoreEventEnum_BTN_LEFT,
  /**
   Once rigth button is pressed we emmit BTN_RIGTH.
   */
  ebk_CoreEventEnum_BTN_RIGTH,
  /**
   Once up button is pressed we emmit BTN_UP.
   */
  ebk_CoreEventEnum_BTN_UP,
  /**
   Once down button is pressed we emmit BTN_DOWN.
   */
  ebk_CoreEventEnum_BTN_DOWN,
  /**
   Every time we want to show books to the user we need to refresh books state
   in the device. User could add book to the storage, so we need to ensure that
   our view of the books is always valid.
   */
  ebk_CoreEventEnum_BOOKS_REFRESHED,

  // Add more events here
  
  /**
   Once any module meet error that cannot be handled we emmit ERROR_RAISED.
   */
  ebk_CoreEventEnum_ERROR_RAISED,
};

/**
  @brief Post event to the core.
  @param core Pointer to initialized instance of core.
  @param event Event to post.
  @param data Generic data in case event needs payload.
 */
void ebk_core_event_post(ebk_core_t core, enum ebk_CoreEventEnum event,
                         void *data);

/**
   @brief Open appropriate core state.
   @param core Core instance to set state in.
   @param state State to open.
   @return 0 on success and ebk_error_t on error.

   Open changes state of the core and close the old state.
*/
ebk_error_t ebk_core_sopen(ebk_core_t core, enum ebk_CoreStateEnum state,
                           void *config);

/**
   @brief Dump state to string.
   @param state State to dump.
   @return Null terminated string.
*/
const char *ebk_core_sdump(enum ebk_CoreStateEnum state);

/**
   @brief Dump event to string.
   @param state Event to dump.
   @return Null terminated string.
*/
const char *ebk_core_edump(enum ebk_CoreEventEnum event);

/**
   @brief Post ERROR_RAISED event.
   @param core Core instance to receive the event.
   @param error Error which will be handled by error module.
*/
void ebk_core_raise_error(ebk_core_t core, ebk_error_t error);

#endif // EBOOK_READER_CORE_INTERNAL_H
